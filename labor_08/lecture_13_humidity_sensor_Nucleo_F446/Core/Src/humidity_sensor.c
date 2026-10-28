/*
 * humidity_sensor.c
 */

#include "main.h"
#include "i2c.h"
#include "humidity_sensor.h"
#include <math.h>

// Read both the humidity and the temperature value measured by the sensor.
HAL_StatusTypeDef HumTempSensor_GetValues(float* humidity, float* temperature)
{
	uint8_t receive_buffer[4];
	HAL_StatusTypeDef stat_tx, stat_rx;

	uint16_t humidity_reg;
	uint16_t temperature_reg;

	/*
	 * Send Measurement Request (MR) to the sensor, then wait for
	 * TempHum_Sensor_MEASURE_DELAY milliseconds for the measurement
	 * to complete, then read the value.
	 */
	stat_tx = HAL_I2C_Master_Transmit(&hi2c2, TempHum_Sensor_ADDRESS, NULL, 0, TempHum_Sensor_TIMEOUT);
	HAL_Delay(TempHum_Sensor_MEASURE_DELAY);
	stat_rx = HAL_I2C_Master_Receive(&hi2c2, TempHum_Sensor_ADDRESS, receive_buffer, 4, TempHum_Sensor_TIMEOUT);

	/*
	 * If the state indicators are not S1 = 0 and S0 = 0, then
	 * the measurement was not successful.
	 */
	if((receive_buffer[0] & 0xC0) != 0) return HAL_ERROR;
	receive_buffer[0] &= 0x3F;

	/*
	 * Mask the humidity and temperature data from the
	 * received data bytes.
	 */
	humidity_reg = (receive_buffer[0]<<8) + receive_buffer[1];
	temperature_reg = ((receive_buffer[2]<<8) + receive_buffer[3])>>2;

	/*
	 * Calculate the actual humidity (%) and temperature (degrees)
	 * values according to the equations given in the datasheet.
	 *
	 * Refer to the Conversion Formulas on the 6th page of the
	 * datasheet under Table 2.
	 */
	if(humidity != NULL) *humidity = (((float)humidity_reg)/((float)pow(2,14)-2))*100;
	if(temperature != NULL) *temperature = ((((float)temperature_reg)/((float)pow(2,14)-2))*165) - 40;

	return (stat_tx == HAL_OK && stat_rx == HAL_OK) ? HAL_OK : HAL_ERROR;

}

// Read the temperature value measured by the sensor.
HAL_StatusTypeDef HumTempSensor_GetTemperature(float* temperature)
{
	/*
	 * The sensor will anyways return both humidity and temperature
	 * data, so use the previously written GetValues function, but
	 * pass a NULL pointer for humidity.
	 */
	return HumTempSensor_GetValues(NULL, temperature);
}

// Read the humidity value measured by the sensor.
HAL_StatusTypeDef HumTempSensor_GetHumidity(float* humidity)
{
	/*
	 * The sensor will anyways return both humidity and temperature
	 * data, so use the previously written GetValues function, but
	 * pass a NULL pointer for humidity.
	 */
	return HumTempSensor_GetValues(humidity, NULL);
}

// Read the status of the sensor.
HAL_StatusTypeDef HumTempSensor_GetStatus(uint8_t* status)
{
	uint8_t receive_buffer[4];
	HAL_StatusTypeDef stat_tx, stat_rx;

	/*
	 * A measurement has to be done to receive the
	 * status bytes with the measured data.
	 */
	stat_tx = HAL_I2C_Master_Transmit(&hi2c2, TempHum_Sensor_ADDRESS, NULL, 0, TempHum_Sensor_TIMEOUT);
	HAL_Delay(TempHum_Sensor_MEASURE_DELAY);
	stat_rx = HAL_I2C_Master_Receive(&hi2c2, TempHum_Sensor_ADDRESS, receive_buffer, 4, TempHum_Sensor_TIMEOUT);

	*status = receive_buffer[0] & 0xC0;
	return (stat_tx == HAL_OK && stat_rx == HAL_OK) ? HAL_OK : HAL_ERROR;

}
