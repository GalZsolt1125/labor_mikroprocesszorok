/*
 * temperature_sensor.h
 */

#ifndef INC_TEMPERATURE_SENSOR_H_
#define INC_TEMPERATURE_SENSOR_H_

#include "main.h"

/*
 * I2C base address and default timeout
 * for the LM75A temperature sensor.
 */
#define LM75A_BASEADDRESS 0x90
#define LM75A_TIMEOUT 1000

/*
 * LM75A register addresses
 */
#define LM75A_REG_CONF         0x01 // Configuration register
#define LM75A_REG_TEMP         0x00 // Temperature register
#define LM75A_REG_TOS          0x03 // Overtemperature Shutdown register
#define LM75A_REG_THYST        0x02 // Temperature Hysteresis register

/*
 * Symbols for the fault queue.
 *
 * The number of faults that has to happen
 * consecutively to activate the overtemperature
 * shutdown output of the register.
 */
#define LM75A_OS_F_QUE_1       0x00
#define LM75A_OS_F_QUE_2       0x01
#define LM75A_OS_F_QUE_4       0x02
#define LM75A_OS_F_QUE_6       0x03
#define LM75A_OS_F_QUE_MASK    0x18 // Mask for the position of the flags
#define LM75A_OS_F_QUE_POS     3    // Bits shifting needed to align the flag to the right

/*
 * Symbols for setting the polarity of the
 * overtemperature shutdown output of the sensor.
 */
#define LM75A_OS_POL_LOW       0x00
#define LM75A_OS_POL_HIGH      0x01
#define LM75A_OS_POL_MASK      0x04
#define LM75A_OS_POL_POS       2

/*
 * Symbols for overtemperature shutdown operation
 * mode selection.
 *
 * The difference between the two modes is that in
 * comparator mode, the OS output becomes active
 * when Temp has exceeded Tos and reset when Temp
 * has dropped below Thyst, reading a register or
 * putting the device into shutdown mode does not
 * change the state of the OS output; while in OS
 * interrupt mode, once it has been activated either
 * by exceeding Tos or dropping below Thyst, the OS
 * output will remain active indefinitely until
 * reading a register, then the OS output is reset.
 *
 * Refer to the datasheet 7.6 on page 9.
 */
#define LM75A_OS_COMP          0x00 // Comparator mode
#define LM75A_OS_INT           0x01 // Interrupt mode
#define LM75A_OS_COMP_INT_MASK 0x02
#define LM75A_OS_COMP_INT_POS  1

/*
 * Symbols for setting the device operation mode.
 */
#define LM75A_NORMAL           0x00
#define LM75A_SHOUTDOWN        0x01
#define LM75A_SHOUTDOWM_MASK   0x01


// In order to ignore 0.5 Celsius
#define LM75A_TOS_REG_SHIFT    8
#define LM75A_THYST_REG_SHIFT  8



/*
 * @brief Write a single instruction to the sensors
 *        selected register.
 *
 * @param The register to be written. Can be one of
 *        LM75A_REG_XXXX constants.
 *
 * @param The new value for the register.
 */
HAL_StatusTypeDef LM75A_Write(uint8_t reg, uint8_t value);

/*
 * @brief Write word to the sensors selected register.
 *
 * @param The register to be written. Can be one of
 *        LM75A_REG_XXXX constants.
 *
 * @param The new value for the register.
 */
HAL_StatusTypeDef LM75A_WriteWord(uint8_t reg, uint16_t value);

/*
 * @brief Read a single byte from the sensors
 *        selected register.
 *
 * @param The register to be read. Can be one of
 *        LM75A_REG_XXXX constants.
 *
 * @param Pointer to an uint8_t variable that will
 *        receive the value.
 */
HAL_StatusTypeDef LM75A_Read(uint8_t reg, uint8_t* value);

/*
 * @brief Read a word from the sensors selected register.
 *
 * @param The register to be read. Can be one of
 *        LM75A_REG_XXXX constants.
 *
 * @param Pointer to an uint16_t variable that will
 *        receive the value.
 */
HAL_StatusTypeDef LM75A_ReadWord(uint8_t reg, uint16_t* value);

/*
 * @brief Read the measured temperature form the sensor.
 *
 * @param Pointer to an uint16_t variable that will
 *        receive the value in degrees.
 */
HAL_StatusTypeDef LM75A_GetTemperature(float* temp);

/*
 * @brief Set the fault queue value.
 *
 * @param The number of faults that has to happen
 *        consecutively to activate the overtemperature
 *        shutdown output of the register. Can be one of
 *        LM75A_OS_F_XXXX constants.
 */
HAL_StatusTypeDef LM75A_SetOSFaultQueue(uint8_t queue);

/*
 * @brief Set the polarity of the overtemperature
 *        shutdown output.
 *
 * @param The polarity to be set. Can be one of
 *        LM75A_OS_POL_XXXX constants.
 */
HAL_StatusTypeDef LM75A_SetOSPolarity(uint8_t polarity);

/*
 * @brief Set the overtemperature shutdown
 *        outputs operation mode.
 *
 * @param The operation mode to be set. Can be either
 *        LM75A_INT or LM75A_COMP.
 */
HAL_StatusTypeDef LM75A_SetOSMode(uint8_t mode);

/*
 * @brief Set the overtemperature shutdown output
 *        trigger value.
 *
 * @param The temperature to be set in degrees.
 */
HAL_StatusTypeDef LM75A_SetOSTemperature(uint16_t temperature);

/*
 * @brief Set the overtemperature shutdown output
 *        hysteresis trigger value.
 *
 * @param The hysteresis to be set in degrees.
 */
HAL_StatusTypeDef LM75A_SetOSHysteresis(uint16_t hysteresis);

/*
 * @brief Set the sensors operation mode.
 *
 * @param The operation mode to be set. Can be either
 *        LM75A_NORMAL or LM75A_SHOUTDOWN.
 */
HAL_StatusTypeDef LM75A_SetShoutdownMode(uint16_t mode);

#endif /* INC_TEMPERATURE_SENSOR_H_ */
