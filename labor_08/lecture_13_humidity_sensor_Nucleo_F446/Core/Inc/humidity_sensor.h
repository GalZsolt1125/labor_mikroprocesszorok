/*
 * humidity_sensor.h
 */

#ifndef INC_HUMIDITY_SENSOR_H_
#define INC_HUMIDITY_SENSOR_H_

#include "main.h"

/*
 * Temperature and humidity sensor address on I2C bus
 */
#define TempHum_Sensor_ADDRESS 0x4E

/*
 * Default I2C communication timeout for the
 * temperature and humidity sensor.
 */
#define TempHum_Sensor_TIMEOUT 1000

/*
 * After sending a measurement request (MR) to the
 * temperature and humidity sensor, it takes some
 * time to make the measurement. During that time
 * it's not possible to read the data from its
 * output register. This is the waiting time for
 * the measurement to finish.
 */
#define TempHum_Sensor_MEASURE_DELAY 50



/*
 * @brief Read both the humidity and the temperature value
 *        measured by the sensor.
 *
 * @param A pointer to a variable that will receive the
 *        humidity measured in percent on 14 bits.
 *
 * @param A pointer to a variable that will receive the
 *        temperature measured in degrees on 14 bits.
 */
HAL_StatusTypeDef HumTempSensor_GetValues(float* humidity, float* temperature);

/*
 * @brief Read the temperature value measured by the sensor.
 *
 * @param A pointer to a variable that will receive the
 *        temperature measured in degrees on 14 bits.
 */
HAL_StatusTypeDef HumTempSensor_GetTemperature(float* temperature);

/*
 * @brief Read the humidity value measured by the sensor.
 *
 * @param A pointer to a variable that will receive the
 *        humidity measured in percent on 14 bits.
 */
HAL_StatusTypeDef HumTempSensor_GetHumidity(float* humidity);

/*
 * @brief Read the status of the sensor.
 *
 * @note Possible status values:
 *       S1 = 0 ; S0 = 0:
 *       	Normal Operation, Valid Data that has not
 *       	been fetched since the last measurement cycle.
 *       S1 = 0 ; S0 = 1:
 *       	Stale Data: Data that has already been fetched
 *       	since the last measurement cycle, or data
 *       	fetched before the first measurement has
 *       	been completed.
 *       S1 = 1 ; S0 = 0:
 *       	Device in Command Mode.
 *       S1 = 1 ; S0 = 1:
 *       	Not Used.
 *
 * @param A pointer to a variable that will receive
 *        the status information aligned to the right.
 */
HAL_StatusTypeDef HumTempSensor_GetStatus(uint8_t* status);

#endif /* INC_HUMIDITY_SENSOR_H_ */
