#ifndef __OLED_CONF_H__
#define __OLED_CONF_H__

// I2C konfiguráció
#define OLED_I2C_PORT        hi2c1
#define OLED_I2C_ADDR        (0x3C << 1)

// Kijelző tükrözése, amennyiben szükséges
// #define OLED_MIRROR_VERT
// #define OLED_MIRROR_HORIZ

// Inverz színbeállítás, amennyiben szükséges
// # define OLED_INVERSE_COLOR

// Csak a szükséges betűtípusok hozzáadása
#define OLED_INCLUDE_FONT_6x8
#define OLED_INCLUDE_FONT_7x10
#define OLED_INCLUDE_FONT_11x18
#define OLED_INCLUDE_FONT_16x26

#endif /* __OLED_CONF_H__ */
