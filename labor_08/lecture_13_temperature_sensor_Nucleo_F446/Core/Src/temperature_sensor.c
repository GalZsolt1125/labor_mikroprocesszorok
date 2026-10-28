/*
 * temperature_sensor.c
 */

#include "temperature_sensor.h"
#include "main.h"
#include "i2c.h"

// Write a single instruction to the sensors selected register.
HAL_StatusTypeDef LM75A_Write(uint8_t reg, uint8_t value)
{
	uint8_t data[2];
	HAL_StatusTypeDef status;

	data[0] = reg;
	data[1] = value;

	status = HAL_I2C_Master_Transmit(&hi2c2, LM75A_BASEADDRESS, data, 2, LM75A_TIMEOUT);

	return status;
}

// Write word to the sensors selected register.
HAL_StatusTypeDef LM75A_WriteWord(uint8_t reg, uint16_t value)
{
	uint8_t data[3];
	HAL_StatusTypeDef status;

	data[0] = reg;
	data[1] = (uint8_t)(value>>8);
	data[2] = (uint8_t)value;

	status = HAL_I2C_Master_Transmit(&hi2c2, LM75A_BASEADDRESS, data, 3, LM75A_TIMEOUT);

	return status;
}

// Read a single byte from the sensors selected register.
HAL_StatusTypeDef LM75A_Read(uint8_t reg, uint8_t* value)
{
	HAL_StatusTypeDef status_tx, status_rx;

	status_tx = HAL_I2C_Master_Transmit(&hi2c2, LM75A_BASEADDRESS, &reg, 1, LM75A_TIMEOUT);
	status_rx = HAL_I2C_Master_Receive(&hi2c2, LM75A_BASEADDRESS, value, 1, LM75A_TIMEOUT);

	return (status_tx == HAL_OK && status_rx == HAL_OK) ? HAL_OK : HAL_ERROR;
}

// Read a word from the sensors selected register.
HAL_StatusTypeDef LM75A_ReadWord(uint8_t reg, uint16_t* val)
{
	uint8_t receive_buffer[2];
	HAL_StatusTypeDef stat_tx, stat_rx;

	stat_tx = HAL_I2C_Master_Transmit(&hi2c2, LM75A_BASEADDRESS, &reg, 1, LM75A_TIMEOUT);

	stat_rx = HAL_I2C_Master_Receive(&hi2c2, LM75A_BASEADDRESS, receive_buffer, 2, LM75A_TIMEOUT);

	*val = ((uint16_t)receive_buffer[0]<<8) + receive_buffer[1];

	return (stat_tx == HAL_OK && stat_rx == HAL_OK) ? HAL_OK : HAL_ERROR;

}

// Read the measured temperature form the sensor.
HAL_StatusTypeDef LM75A_GetTemperature(float* temp)
{
    uint16_t temp_register, temp_temporary;
	HAL_StatusTypeDef status;

	status = LM75A_ReadWord(LM75A_REG_TEMP, &temp_register);

	/*
	 * The temperature data is stored on 10 bits of the register
	 * on D15..D5, so it has to be shifted down 5 bits.
	 */
    temp_temporary = temp_register>>5;
    /*
     * The temperature value is stored in 0.125 degrees resolution,
     * so the value has to be multiplied by 0.125 to get the
     * temperature in degrees.
     */
    *temp = (float)(temp_temporary) * 0.125;

    return status;
}

// Set the fault queue value.
HAL_StatusTypeDef LM75A_SetOSFaultQueue(uint8_t queue)
{
	uint8_t regState;
	HAL_StatusTypeDef status_tx, status_rx;

	status_tx = LM75A_Read(LM75A_REG_CONF, &regState);
	regState &= ~LM75A_OS_F_QUE_MASK;
	regState |= queue<<LM75A_OS_F_QUE_POS;
	status_rx =LM75A_Write(LM75A_REG_CONF, regState);

	return (status_tx == HAL_OK && status_rx == HAL_OK) ? HAL_OK : HAL_ERROR;
}

// Set the polarity of the overtemperature shutdown output.
HAL_StatusTypeDef LM75A_SetOSPolarity(uint8_t polarity)
{
	uint8_t regState;
	HAL_StatusTypeDef status_tx, status_rx;

	status_tx = LM75A_Read(LM75A_REG_CONF, &regState);
	regState &= ~LM75A_OS_POL_MASK;
	regState |= polarity<<LM75A_OS_POL_POS;
	status_rx =LM75A_Write(LM75A_REG_CONF, regState);

	return (status_tx == HAL_OK && status_rx == HAL_OK) ? HAL_OK : HAL_ERROR;
}

// Set the overtemperature shutdown outputs operation mode.
HAL_StatusTypeDef LM75A_SetOSMode(uint8_t mode)
{
	uint8_t regState;
	HAL_StatusTypeDef status_tx, status_rx;

	status_tx = LM75A_Read(LM75A_REG_CONF, &regState);
	regState &= ~LM75A_OS_COMP_INT_MASK;
	regState |= mode<<LM75A_OS_COMP_INT_POS;
	status_rx =LM75A_Write(LM75A_REG_CONF, regState);

	return (status_tx == HAL_OK && status_rx == HAL_OK) ? HAL_OK : HAL_ERROR;
}

// Set the overtemperature shutdown output trigger value.
HAL_StatusTypeDef LM75A_SetOSTemperature(uint16_t temperature)
{
	HAL_StatusTypeDef status;

	temperature <<= LM75A_TOS_REG_SHIFT;

	status = LM75A_WriteWord(LM75A_REG_TOS, temperature);

	return status;
}

// Set the overtemperature shutdown output hysteresis trigger value.
HAL_StatusTypeDef LM75A_SetOSHysteresis(uint16_t hysteresis)
{
	HAL_StatusTypeDef status;

	hysteresis <<= LM75A_THYST_REG_SHIFT;

	status = LM75A_WriteWord(LM75A_REG_THYST, hysteresis);

	return status;
}

// Set the sensors operation mode.
HAL_StatusTypeDef LM75A_SetShoutdownMode(uint16_t mode){
	uint8_t regState;
	HAL_StatusTypeDef status_tx, status_rx;

	status_tx = LM75A_Read(LM75A_REG_CONF, &regState);
	regState &= ~LM75A_SHOUTDOWM_MASK;
	regState |= mode;
	status_rx =LM75A_Write(LM75A_REG_CONF, regState);

	return (status_tx == HAL_OK && status_rx == HAL_OK) ? HAL_OK : HAL_ERROR;
}
