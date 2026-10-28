/*
 * light_sensor.h
 */

#ifndef INC_LIGHT_SENSOR_H_
#define INC_LIGHT_SENSOR_H_

#include "main.h"

/*
 * Possible I2C addresses for the light sensor.
 *
 * The address select pin of the light sensor IC can be
 * connected to VDD or GND using the connection on the
 * extension pin header of the board. The ADDR_SEL pin
 * is labeled ADDRSEL on the board.
 */
#define LightSensor_ADDR_GND           0x52 // ADDR_SEL is tied to GND
#define LightSensor_ADDR_FLOAT         0x72 // ADDR_SEL is left floating
#define LightSensor_ADDR_VDD           0x92 // ADDR_SEL is tied to VDD

/*
 * Default timeout for the I2C communication with
 * the light sensor.
 */
#define LightSensor_Timeout 1000

/*
 * Register addresses of the light sensor.
 */
#define LightSensor_REG_CONTROL        0x00
#define LightSensor_REG_TIMING         0x01
#define LightSensor_REG_THRESHLOWLOW   0x02
#define LightSensor_REG_THRESHLOWHIGH  0x03
#define LightSensor_REG_THRESHHIGHLOW  0x04
#define LightSensor_REG_THRESHHIGHHIGH 0x05
#define LightSensor_REG_INTERRUPT      0x06
#define LightSensor_REG_CRC            0x08
#define LightSensor_REG_ID             0x0A
#define LightSensor_REG_DATA0LOW       0x0C
#define LightSensor_REG_DATA0HIGH      0x0D
#define LightSensor_REG_DATA1LOW       0x0E
#define LightSensor_REG_DATA1HIGH      0x0F

/*
 * Setting for the I2C, tells the IC if the
 * data is going to be a single byte or a word.
 */
#define LightSensor_CMDREG_CMD         0x80
#define LightSensor_CMDREG_CLEARINT    0x40
#define LightSensor_CMDREG_WORD        0x20

/*
 * Symbols for power state setting of the
 * light sensor IC.
 */
#define LightSensor_POWER_ON           0x03
#define LightSensor_POWER_OFF          0x00

/*
 * Symbols for selecting the gain of the
 * amplifier in the light sensors ADC.
 */
#define LightSensor_GAIN_1X            0x00
#define LightSensor_GAIN_16X           0x10
#define LightSensor_GAIN_MASK          0xEF
#define LightSensor_GAIN_POS		   4

/*
 * Symbols for selecting the integration
 * time of the ADC in the light sensor.
 */
#define LightSensor_INTEG_13m7         0x00
#define LightSensor_INTEG_101m         0x01
#define LightSensor_INTEG_402m         0x02
#define LightSensor_INTEG_MASK         0xFC

/*
 * Symbols for setting the interrupt mode
 * of the light sensor.
 */
#define LightSensor_INT_OFF            0x00
#define LightSensor_INT_ON             0x10
#define LightSensor_INT_MASK		   0xCF

/*
 * The number of consecutive times the
 * measured light level has to be higher
 * than the threshold to activate the
 * interrupt of the MCU.
 */
#define LightSensor_PERSIST_EVERY      0x00
#define LightSensor_PERSIST_ANY        0x01
#define LightSensor_PERSIST_2CYCLE     0x02
#define LightSensor_PERSIST_3CYCLE     0x03
#define LightSensor_PERSIST_4CYCLE     0x04
#define LightSensor_PERSIST_5CYCLE     0x05
#define LightSensor_PERSIST_6CYCLE     0x06
#define LightSensor_PERSIST_7CYCLE     0x07
#define LightSensor_PERSIST_8CYCLE     0x08
#define LightSensor_PERSIST_9CYCLE     0x09
#define LightSensor_PERSIST_10CYCLE    0x0A
#define LightSensor_PERSIST_11CYCLE    0x0B
#define LightSensor_PERSIST_12CYCLE    0x0C
#define LightSensor_PERSIST_13CYCLE    0x0D
#define LightSensor_PERSIST_14CYCLE    0x0E
#define LightSensor_PERSIST_15CYCLE    0x0F
#define LightSensor_PERSIST_MASK	   0xF0



/*
 * Global variable that holds the light sensors I2C address.
 * Can be one of the LightSensor_ADDR_XXXX constants.
 *
 * The address select pin of the light sensor IC can be
 * connected to VDD or GND using the connection on the
 * extension pin header of the board. The ADDR_SEL pin
 * is labeled ADDRSEL on the board.
 */
extern uint8_t LightSensor_Address;/*

 * @brief Clear the interrupt bit in the sensor.
 */
HAL_StatusTypeDef LightSensor_ClearInterrupt();

/*
 * @brief Write a command to the sensor.
 *
 * @param The command.
 */
HAL_StatusTypeDef LightSensor_Cmd(uint8_t cmd);

/*
 * @brief Set the I2C address of the IC.
 *
 * @param The address that is selected by connecting the
 *        ADDR_SEL pin of the IC to VDD or GND, or left
 *        floating.
 *
 */
void LightSensor_SetAddress(uint8_t address);

/*
 * @brief Write a single instruction to the sensors
 *        selected register.
 *
 * @param The register to be written. Can be one of
 *        LightSensor_REG_XXXX constants.
 *
 * @param The new value for the register.
 */
HAL_StatusTypeDef LightSensor_Write(uint8_t reg, uint8_t value);

/*
 * @brief Write word to the sensors selected register.
 *
 * @param The register to be written. Can be one of
 *        LightSensor_REG_XXXX constants.
 *
 * @param The new value for the register.
 */
HAL_StatusTypeDef LightSensor_WriteWord(uint8_t reg, uint16_t value);

/*
 * @brief Read a single byte from the sensors
 *        selected register.
 *
 * @param The register to be read. Can be one of
 *        LightSensor_REG_XXXX constants.
 *
 * @param Pointer to an uint8_t variable that will
 *        receive the value.
 */
HAL_StatusTypeDef LightSensor_Read(uint8_t reg, uint8_t* value);

/*
 * @brief Set the power state of the sensor.
 *
 * @param The power state to be set. Can be
 *        LightSensor_POWER_On or LightSensor_POWER_OFF
 *        constant.
 */
HAL_StatusTypeDef LightSensor_SetPowerState(uint8_t state);

/*
 * @brief Set the gain of the ADC in the sensor.
 *
 * @param The gain to be set. Can be LightSensor_GAIN_1X
 *        or LightSensor_GAIN_16X constant.
 */
HAL_StatusTypeDef LightSensor_SetGain(uint8_t gain);

/*
 * @brief Set the integration time of the ADC in the sensor.
 *
 * @param The integration time to be set. Can be one of
 *        LightSensor_INTEG_XXXX constants.
 */
HAL_StatusTypeDef LightSensor_SetIntegrationTime(uint8_t time);

/*
 * @brief Set the interrupt state of the sensor.
 *
 * @param The state to be set. Can be LightSensor_INT_OFF
 *        or LightSensor_INT_ON constant.
 */
HAL_StatusTypeDef LightSensor_SetInterruptState(uint8_t state);

/*
 * @brief Set the interrupt persistence of the sensor.
 *
 * @param The persistence to be set. Can be one of
 *        LightSensor_PERSIST_XXXX constants.
 */
HAL_StatusTypeDef LightSensor_SetInterruptPersist(uint8_t persist);

/*
 * @brief Set the interrupt thresholds.
 *
 * @param The lower threshold to be set.
 *
 * @param The upper threshold to be set.
 */
HAL_StatusTypeDef LightSensor_SetThreshold(uint16_t lower, uint16_t upper);

/*
 * @brief Get the integration time of the ADC in the sensor.
 *
 * @param A pointer to a variable that will receive the
 *        integration time that is set. Will be one of
 *        LightSensor_INTEG_XXXX constants.
 */
HAL_StatusTypeDef LightSensor_GetIntegrationTime(uint8_t* regVal);

/*
 * @brief Get the gain of the ADC in the sensor.
 *
 * @param A pointer to a variable that will receive the
 *        gain that is set. Will be LightSensor_GAIN_1X
 *        or LightSensor_GAIN_16x.
 */
HAL_StatusTypeDef LightSensor_GetGain(uint8_t* regVal);

/*
 * @brief Read the sensors channel 0 value. This channel
 *        measures both the visible and the IR light.
 *
 * @param A pointer to a variable that will receive the
 *        light level that is measured.
 */
HAL_StatusTypeDef LightSensor_GetCH0Value(uint16_t* value);

/*
 * @brief Read the sensors channel 1 value. This channel
 *        measures only the IR light.
 *
 * @param A pointer to a variable that will receive the
 *        light level that is measured.
 */
HAL_StatusTypeDef LightSensor_GetCH1Value(uint16_t* value);

/*
 * @brief Read the light level the sensor measures in lux.
 *
 * @param A pointer to a variable that will receive the
 *        light level that is measured.
 */
HAL_StatusTypeDef LightSensor_GetLightLevel(float* lightLevel);

#endif /* INC_LIGHT_SENSOR_H_ */
