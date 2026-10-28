/*
 * light_sensor.c
 */

#include "i2c.h"
#include "main.h"
#include "light_sensor.h"
#include <math.h>

// Default address definition
uint8_t LightSensor_Address = LightSensor_ADDR_FLOAT;

// Function for changing the address
void LightSensor_SetAddress(uint8_t address)
{
	LightSensor_Address = address;
}

// Send command
HAL_StatusTypeDef LightSensor_Cmd(uint8_t cmd)
{
	HAL_StatusTypeDef status;

	status = HAL_I2C_Master_Transmit(&hi2c2, LightSensor_Address, &cmd, 1, LightSensor_Timeout);

	return status;
}

// Write a byte to a selected register
HAL_StatusTypeDef LightSensor_Write(uint8_t reg, uint8_t value)
{
	uint8_t data[2];
	HAL_StatusTypeDef status;

	data[0] = reg | LightSensor_CMDREG_CMD;
	data[1] = value;

	status = HAL_I2C_Master_Transmit(&hi2c2, LightSensor_Address, data, 2, LightSensor_Timeout);

	return status;

}

// Write a word to a selected register
HAL_StatusTypeDef LightSensor_WriteWord(uint8_t reg, uint16_t value)
{
	uint8_t data[3];
	HAL_StatusTypeDef status;

	data[0] = LightSensor_CMDREG_CMD | LightSensor_CMDREG_WORD | reg;
	// Type conversion does not required, but in that way it is more clear
	data[1] = (uint8_t)(value); // value & 0x00FF gives the same result
	data[2] = (uint8_t)(value >> 8);

	status = HAL_I2C_Master_Transmit(&hi2c2, LightSensor_Address, data, 3, LightSensor_Timeout);

	return status;

}
// Read a byte from a selected register
HAL_StatusTypeDef LightSensor_Read(uint8_t reg, uint8_t* value)
{
	uint8_t data_tx;
	HAL_StatusTypeDef status_tx, status_rx;

	data_tx = reg | LightSensor_CMDREG_CMD;

	status_tx = HAL_I2C_Master_Transmit(&hi2c2, LightSensor_Address, &data_tx, 1, LightSensor_Timeout);
	status_rx = HAL_I2C_Master_Receive(&hi2c2, LightSensor_Address, value, 1, LightSensor_Timeout);

	return (status_tx == HAL_OK && status_rx == HAL_OK) ? HAL_OK : HAL_ERROR;

}
// Set the selected power state
HAL_StatusTypeDef LightSensor_SetPowerState(uint8_t state)
{
	HAL_StatusTypeDef status;
	status = LightSensor_Write(LightSensor_REG_CONTROL, state);

	return status;

}
// Set the selected gain
HAL_StatusTypeDef LightSensor_SetGain(uint8_t gain)
{
	uint8_t regState;
	HAL_StatusTypeDef status_tx, status_rx;

	status_tx = LightSensor_Read(LightSensor_REG_TIMING, &regState);
	regState &= LightSensor_GAIN_MASK;
	regState |= gain;
	status_rx =LightSensor_Write(LightSensor_REG_TIMING, regState);

	return (status_tx == HAL_OK && status_rx == HAL_OK) ? HAL_OK : HAL_ERROR;

}
// Set the selected integration time
HAL_StatusTypeDef LightSensor_SetIntegrationTime(uint8_t time)
{
	uint8_t regState;
	HAL_StatusTypeDef status_tx, status_rx;

	status_tx = LightSensor_Read(LightSensor_REG_TIMING, &regState);
	regState &= LightSensor_INTEG_MASK;
	regState |= time;
	status_rx = LightSensor_Write(LightSensor_REG_TIMING, regState);

	return (status_tx == HAL_OK && status_rx == HAL_OK) ? HAL_OK : HAL_ERROR;

}
// Clear the new interrupts
HAL_StatusTypeDef LightSensor_ClearInterrupt()
{
	HAL_StatusTypeDef status;

	status = LightSensor_Cmd(LightSensor_CMDREG_CMD | LightSensor_CMDREG_CLEARINT);

	return status;
}
// Set the interrupt state
HAL_StatusTypeDef LightSensor_SetInterruptState(uint8_t state)
{
	uint8_t regState;
	HAL_StatusTypeDef status_tx, status_rx;

	status_tx = LightSensor_Read(LightSensor_REG_INTERRUPT, &regState);
	regState &= LightSensor_INT_MASK;
	regState |= state;
	status_rx = LightSensor_Write(LightSensor_REG_INTERRUPT, regState);

	return (status_tx == HAL_OK && status_rx == HAL_OK) ? HAL_OK : HAL_ERROR;
}
// Set the persistence of the interrupt
HAL_StatusTypeDef LightSensor_SetInterruptPersist(uint8_t persist)
{
	uint8_t regState;
	HAL_StatusTypeDef status_tx, status_rx;

	status_tx = LightSensor_Read(LightSensor_REG_INTERRUPT, &regState);
	regState &= LightSensor_PERSIST_MASK;
	regState |= persist;
	status_rx = LightSensor_Write(LightSensor_REG_INTERRUPT, regState);

	return (status_tx == HAL_OK && status_rx == HAL_OK) ? HAL_OK : HAL_ERROR;

}
// Set the treshold levels of the sensor
HAL_StatusTypeDef LightSensor_SetThreshold(uint16_t lower, uint16_t upper)
{
	HAL_StatusTypeDef status_lower, status_upper;

	uint8_t pData_lower[3];
	uint8_t pData_upper[3];

	pData_lower[0] = LightSensor_CMDREG_CMD | LightSensor_CMDREG_WORD | LightSensor_REG_THRESHLOWLOW;
	pData_lower[1] = lower & 0x00FF;
	pData_lower[2] = lower >> 8;

	pData_upper[0] = LightSensor_CMDREG_CMD | LightSensor_CMDREG_WORD | LightSensor_REG_THRESHHIGHLOW;
	pData_upper[1] = upper & 0x00FF;
	pData_upper[2] = upper >> 8;

	status_lower = HAL_I2C_Master_Transmit(&hi2c2, LightSensor_Address, pData_lower, 3, LightSensor_Timeout);
	status_upper = HAL_I2C_Master_Transmit(&hi2c2, LightSensor_Address, pData_upper, 3, LightSensor_Timeout);

	return (status_lower == HAL_OK && status_upper == HAL_OK) ? HAL_OK : HAL_ERROR;

}
// Read the integration time
HAL_StatusTypeDef LightSensor_GetIntegrationTime(uint8_t* regVal)
{
	HAL_StatusTypeDef status;

	status = LightSensor_Read(LightSensor_REG_TIMING, regVal);
	*regVal &= ~(uint8_t)LightSensor_INTEG_MASK;

	return status;

}
// Read the gain
HAL_StatusTypeDef LightSensor_GetGain(uint8_t* regVal)
{
	HAL_StatusTypeDef status;

	status = LightSensor_Read(LightSensor_REG_TIMING, regVal);
	*regVal &= ~(uint8_t)LightSensor_GAIN_MASK;
//	*regVal = (*regVal)>>LightSensor_GAIN_POS;

	return status;

}
// Read the CH0 value
HAL_StatusTypeDef LightSensor_GetCH0Value(uint16_t* value)
{
	uint8_t data = LightSensor_CMDREG_CMD | LightSensor_CMDREG_WORD | LightSensor_REG_DATA0LOW;
	uint8_t regVal[2];
	HAL_StatusTypeDef stat_tx, stat_rx;

	stat_tx = HAL_I2C_Master_Transmit(&hi2c2, LightSensor_Address, &data, 1, LightSensor_Timeout);
	stat_rx = HAL_I2C_Master_Receive(&hi2c2, LightSensor_Address, regVal, 2, LightSensor_Timeout);

	*value = (regVal[1]<<8) + regVal[0];

	return (stat_tx == HAL_OK && stat_rx == HAL_OK) ? HAL_OK : HAL_ERROR;

}
// Read the CH1 value
HAL_StatusTypeDef LightSensor_GetCH1Value(uint16_t* value)
{
	uint8_t data = LightSensor_CMDREG_CMD | LightSensor_CMDREG_WORD | LightSensor_REG_DATA1LOW;
	uint8_t regVal[2];
	HAL_StatusTypeDef stat_tx, stat_rx;

	stat_tx = HAL_I2C_Master_Transmit(&hi2c2, LightSensor_Address, &data, 1, LightSensor_Timeout);
	stat_rx = HAL_I2C_Master_Receive(&hi2c2, LightSensor_Address, regVal, 2, LightSensor_Timeout);

	*value = (regVal[1]<<8) + regVal[0];

	return (stat_tx == HAL_OK && stat_rx == HAL_OK) ? HAL_OK : HAL_ERROR;

}
// Read the light level
HAL_StatusTypeDef LightSensor_GetLightLevel(float* lightLevel)
{
	uint16_t CH0_value_int, CH1_value_int;
	uint8_t gain;
	uint8_t integrationTime;

	float CH0_value, CH1_value;
	float ratio;

	HAL_StatusTypeDef status_CH0, status_CH1, status_Gain, status_IntegrationTime;

	/*
	 * Read the CH0 and CH1 value, and also get the gain and integration
	 * time setting, because these are needed to calculate the
	 * actual light level.
	 */
	status_CH0 = LightSensor_GetCH0Value(&CH0_value_int);
	status_CH1 = LightSensor_GetCH1Value(&CH1_value_int);
	status_Gain = LightSensor_GetGain(&gain);
	status_IntegrationTime = LightSensor_GetIntegrationTime(&integrationTime);

	/*
	 * Convert the uint16_t values to float
	 */
	CH0_value = (float)CH0_value_int;
	CH1_value = (float)CH1_value_int;

	/*
	 * Calculate the actual light level in lux according to the
	 * equations in the datasheet.
	 *
	 * Refer to the datasheet notes on page 5 for calculations.
	 */

	// Check is CH0 value is 0 before dividing with it.
	if(CH0_value != 0)
	{
		ratio = CH1_value / CH0_value;
	}
	else
	{
		return HAL_ERROR;
	}

	if(gain == LightSensor_GAIN_1X)
	{
		CH0_value *= 16;
		CH1_value *= 16;
	}

	switch(integrationTime)
	{
		case LightSensor_INTEG_13m7:
			CH0_value *= (322/11);
			CH1_value *= (322/11);
			break;
		case LightSensor_INTEG_101m:
			CH0_value *= (322/81);
			CH1_value *= (322/81);
			break;
		case LightSensor_INTEG_402m:
			CH0_value *= (322/322);
			CH1_value *= (322/322);
			break;
	}
	if(ratio < 0)
	{
		return HAL_ERROR;
	}
	else if(ratio <= 0.52)
	{
		 *lightLevel = (0.0315f * CH0_value) - (0.0593f * CH0_value * (float)(pow(ratio, 1.4)));
	}
	else if(ratio <= 0.65)
	{
		*lightLevel = (0.0229f * CH0_value) - (0.0291f * CH1_value);
	}
	else if(ratio <= 0.80)
	{
		*lightLevel = (0.0157f * CH0_value) - (0.0180f * CH1_value);
	}
	else if(ratio <= 1.3)
	{
		*lightLevel = (0.00338f * CH0_value) - (0.00260f * CH1_value);
	}
	else if(ratio >= 1.3)
	{
		*lightLevel = 0;
	}

	return (status_CH0  == HAL_OK &&
			status_CH1  == HAL_OK &&
			status_Gain == HAL_OK &&
			status_IntegrationTime == HAL_OK) ? HAL_OK : HAL_ERROR;

}
