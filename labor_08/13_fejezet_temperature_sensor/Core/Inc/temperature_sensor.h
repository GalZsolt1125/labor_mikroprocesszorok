/*
 * temperature_sensor.h
 */

#ifndef INC_TEMPERATURE_SENSOR_H_
#define INC_TEMPERATURE_SENSOR_H_

#include "main.h"

/*
 * LM75A hőmérséklet szenzor alapértemezett I2C címe
 * és időkorlátja a kommunikációhoz.
 */
#define LM75A_BASEADDRESS 0x90
#define LM75A_TIMEOUT 1000

/*
 * LM75A regiszter címek
 */
#define LM75A_REG_CONF         0x01 // Konfigurációs regiszter
#define LM75A_REG_TEMP         0x00 // Hőmérsékletet tároló regiszter
#define LM75A_REG_TOS          0x03 // Túl magas hőmérséklet határértékét tároló regiszter
#define LM75A_REG_THYST        0x02 // Hőmérsékleti hiszterézis értékét tároló regiszter

/*
 * Hibasorhoz tartozó felsorolásS
 *
 * Az egymás után bekövetkező túl magas hőmérséklet események
 * száma ahhoz, hogy aktiválódjón a túlhőmérsékleti leállítás kimenet
 */
#define LM75A_OS_F_QUE_1       0x00
#define LM75A_OS_F_QUE_2       0x01
#define LM75A_OS_F_QUE_4       0x02
#define LM75A_OS_F_QUE_6       0x03
#define LM75A_OS_F_QUE_MASK    0x18 // Maszk a flagek pozíciójához
#define LM75A_OS_F_QUE_POS     3    // Bit shiftelés értéke azért, hogy a flagek a jobb oldalra rendeződjenek

/*
 * Szimbólumok a túlhőmérsékleti kimenet polaritásának beállítására
 */
#define LM75A_OS_POL_LOW       0x00
#define LM75A_OS_POL_HIGH      0x01
#define LM75A_OS_POL_MASK      0x04
#define LM75A_OS_POL_POS       2

/*
 * Szimbólumok a túlhőmérsékleti leállítási mód kiválasztására
 *
 * A két üzemmód közötti különbség az, hogy komparátor üzemmódban
 * az OS kimenet akkor válik aktívvá, amikor a Temp meghaladta a
 * Tos értéket, és visszaáll, amikor a Temp érték a Thyst érték alá
 * csökkent, egy regiszter olvasása vagy az eszköz kikapcsolási
 * üzemmódba helyezése nem változtatja meg az OS kimenet állapotát,
 * míg az OS megszakítási üzemmódban, ha egyszer aktiválódik, akár
 * a Tos érték túllépése, akár a Thyst érték alá csökkenése miatt,
 * az OS kimenet korlátlan ideig aktív marad egy regiszter
 * kiolvasásáig, ekkor az OS-kimenet visszaáll.
 *
 * Ez megtalálható az adatlap 7.6-os pontjában a 9. oldalon.
 */
#define LM75A_OS_COMP          0x00 // Komparátoros mód
#define LM75A_OS_INT           0x01 // Megszakításos mód
#define LM75A_OS_COMP_INT_MASK 0x02
#define LM75A_OS_COMP_INT_POS  1

/*
 * Szimbólumok a szenzor üzemmódjainak beállításához
 */
#define LM75A_NORMAL           0x00
#define LM75A_SHOUTDOWN        0x01
#define LM75A_SHOUTDOWM_MASK   0x01


// Definíciók annak érdekében, hogy ignorálni tudjunk 0,5 °C-t
#define LM75A_TOS_REG_SHIFT    8
#define LM75A_THYST_REG_SHIFT  8



/*
 * @brief Egy bájt utasítás írása a szenzor kiválasztott regiszterébe
 *
 * @param Az írni kívánt regiszter, az LM75A_REG_XXXX konstansok valamelyike lehet
 *
 * @param A regiszter új értéke
 */
HAL_StatusTypeDef LM75A_Write(uint8_t reg, uint8_t value);

/*
 * @brief Egy szó utasítás írása a szenzor kiválasztott regiszterébe
 *
 * @param Az írni kívánt regiszter, az LM75A_REG_XXXX konstansok valamelyike lehet
 *
 * @param A regiszter új értéke
 */
HAL_StatusTypeDef LM75A_WriteWord(uint8_t reg, uint16_t value);

/*
 * @brief Egy bájt olvasása a szenzor kiválasztott regiszteréből
 *
 * @param Az olvasni kívánt regiszter, az LM75A_REG_XXXX konstansok valamelyike lehet
 *
 * @param Pointer egy uint8_t típusú változóra, amelybe az értéket írjuk
 */
HAL_StatusTypeDef LM75A_Read(uint8_t reg, uint8_t* value);

/*
 * @brief Egy szó olvasása a szenzor kiválasztott regiszteréből
 *
 * @param Az olvasni kívánt regiszter, az LM75A_REG_XXXX konstansok valamelyike lehet
 *
 * @param Pointer egy uint16_t típusú változóra, amelybe az értéket írjuk
 */
HAL_StatusTypeDef LM75A_ReadWord(uint8_t reg, uint16_t* value);

/*
 * @brief Mért hőmérséklet kiolvasása a szenzorból
 *
 * @param Pointer egy uint16_t típusú változóra, amelybe az értéket írjuk Celsius-fokban
 */
HAL_StatusTypeDef LM75A_GetTemperature(float* temp);

/*
 * @brief Hibasor értékének beállítása
 *
 * @param Egymás után következő hibák száma, amelyre szükség van ahhoz, hogy aktiválódjon
 *        a túlhőmérsékleti kimenet. Értéke a LM75A_OS_F_XXXX konstansok valamelyike lehet.
 */
HAL_StatusTypeDef LM75A_SetOSFaultQueue(uint8_t queue);

/*
 * @brief Túlhőmérsékleti kimenet polaritásának beállítása
 *
 * @param Beállítandó polaritás, értéke az LM75A_OS_POL_XXXX kontansok valamelyike lehet.
 */
HAL_StatusTypeDef LM75A_SetOSPolarity(uint8_t polarity);

/*
 * @brief Túlhőmérsékleti kimenet üzemmódja
 *
 * @param Beállítandó üzemmód, vagy az LM75A_INT vagy az LM75A_COMP lehet
 */
HAL_StatusTypeDef LM75A_SetOSMode(uint8_t mode);

/*
 * @brief Túlhőmérséklet küszöbértékének beállítása
 *
 * @param Beállítandó hőmérséklet Celsius-fokban
 */
HAL_StatusTypeDef LM75A_SetOSTemperature(uint16_t temperature);

/*
 * @brief Túlhőmérséklet hiszterézisének beállítása
 *
 * @param Hiszterézis értéke Celsius-fokban
 */
HAL_StatusTypeDef LM75A_SetOSHysteresis(uint16_t hysteresis);

/*
 * @brief Szenzor üzemmódjának beállítása
 *
 * @param Beállítandó üzemmód, vagy az LM75A_NORMAL vagy az LM75A_SHOUTDOWN lehet
 */
HAL_StatusTypeDef LM75A_SetShoutdownMode(uint16_t mode);

#endif /* INC_TEMPERATURE_SENSOR_H_ */
