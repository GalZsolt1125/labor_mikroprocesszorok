#include <math.h>
#include <OLED.h>
#include <stdlib.h>
#include <string.h>

// Egy bájt küldése a parancsregiszterbe
void OLED_WriteCommand(uint8_t byte) {
    HAL_I2C_Mem_Write(&OLED_I2C_PORT, OLED_I2C_ADDR, 0x00, 1, &byte, 1, HAL_MAX_DELAY);
}

// Adatküldés
void OLED_WriteData(uint8_t* buffer, size_t buff_size) {
    HAL_I2C_Mem_Write(&OLED_I2C_PORT, OLED_I2C_ADDR, 0x40, 1, buffer, buff_size, HAL_MAX_DELAY);
}

// Kijelző buffer létrehozása
static uint8_t OLED_Buffer[OLED_BUFFER_SIZE];

// Kijelző "objektum" létrehozása
static OLED_t SSD1306;

/* Kitölti a kijelző buffert egy adott, rögzített hosszúságú bufferből származó értékekkel */
OLED_Error_t OLED_FillBuffer(uint8_t* buf, uint32_t len)
{
    OLED_Error_t ret = OLED_ERR;

    if (len <= OLED_BUFFER_SIZE) {
        memcpy(OLED_Buffer,buf,len);
        ret = OLED_OK;
    }

    return ret;
}

// OLED kijelző inicializálás
void OLED_Init(void)
{

    // Kijelző bootolására várakozás
    HAL_Delay(100);

    // Inicializálás
    OLED_SetDisplayOn(0); 	 // Kijelző kikapcsolás

    OLED_WriteCommand(0x20); // Memória címzési mód beállítása
    OLED_WriteCommand(0x00); // 0b00 - 0b00 - Vízszintes címzési mód
    						 //	0b01 - Függőleges címzési mód
                             // 0b10 - Lap címzési mód (Visszaállítás)
    						 // 0b11 - Nem érvényes

    OLED_WriteCommand(0xB0); //Kezdő oldal címének beállítása lap címzési módban, 0-7

#ifdef OLED_MIRROR_VERT
    OLED_WriteCommand(0xC0); // A képernyő függőleges tükrözése
#else
    OLED_WriteCommand(0xC8); // COM kimenet letapogatási irányának beállítása
#endif

    OLED_WriteCommand(0x00); // Alacsony oszlopcím beállítása
    OLED_WriteCommand(0x10); // Magas oszlopcím beállítása

    OLED_WriteCommand(0x40); // Kezdő vonal címének beállítása

    OLED_SetContrast(0xFF);

#ifdef OLED_MIRROR_HORIZ
    OLED_WriteCommand(0xA0); // A képernyő vízszintes tükrözése
#else
    OLED_WriteCommand(0xA1); // A szegmens újratérképezésének beállítása 0 és 127 között
#endif

#ifdef OLED_INVERSE_COLOR
    OLED_WriteCommand(0xA7); // Inverz szín beállítás
#else
    OLED_WriteCommand(0xA6); // Normál szín beállítás
#endif

    // Multiplexelési arány beállítása
    OLED_WriteCommand(0xA8); // Multiplexelési arány beállítása(1-től 64-ig)

    OLED_WriteCommand(0x3F); // Képernyő magasságának beállítása 64 pixelre


    OLED_WriteCommand(0xA4); // 0xa4 - A kimenet követi a RAM tartalmát
    						 // 0xa5 - A kimenet ignorálja a RAM tartalmát

    OLED_WriteCommand(0xD3); // Kijelző offset-ének beállítása
    OLED_WriteCommand(0x00); // Nincs offset

    OLED_WriteCommand(0xD5); // A kijelző órajelosztási arányának/oszcillátor frekvenciájának beállítása
    OLED_WriteCommand(0xF0); // Osztási arány beállítása

    OLED_WriteCommand(0xD9); // Előtöltési periódus beállítása
    OLED_WriteCommand(0x22); //

    OLED_WriteCommand(0xDA); // Kommunikációs lábak hardverkonfigurációjának beállítása

    OLED_WriteCommand(0x12);

    OLED_WriteCommand(0xDB); // Vcomh beállítása
    OLED_WriteCommand(0x20); // 0x20 - 0.77xVcc

    OLED_WriteCommand(0x8D); // DC-DC engedélyezés beállítása
    OLED_WriteCommand(0x14);
    OLED_SetDisplayOn(1);    // OLED panel bekapcsolása

    // Képernyő "letisztítása"
    OLED_Fill(Black);
    
    // A buffer ürítése a képernyőre
    OLED_UpdateScreen();
    
    // A kurzor alapértelmezett értékeinek beállítása
    SSD1306.CurrentX = 0;
    SSD1306.CurrentY = 0;
    
    // OLED kijelző inicializálás sikeres volt
    SSD1306.Initialized = 1;
}

// Kijelző adott színre történő kitöltése
void OLED_Fill(OLED_COLOR color)
{
    /* Memória beállítás */
    uint32_t i;

    for(i = 0; i < sizeof(OLED_Buffer); i++) {
        OLED_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
    }
}

// Kijelző frissítése a bufferadatok alapján
void OLED_UpdateScreen(void)
{
    for(uint8_t i = 0; i < OLED_HEIGHT/8; i++)
    {
        OLED_WriteCommand(0xB0 + i); // Aktuális RAM cím kiválasztása
        OLED_WriteCommand(0x00);
        OLED_WriteCommand(0x10);
        OLED_WriteData(&OLED_Buffer[OLED_WIDTH*i],OLED_WIDTH);
    }
}

//    Egy pixel rajzolása a kijelző bufferbe
//    X => X koordináta
//    Y => Y koordináta
//    color => Pixel színe
void OLED_DrawPixel(uint8_t x, uint8_t y, OLED_COLOR color)
{
    if(x >= OLED_WIDTH || y >= OLED_HEIGHT)
    {
        // Ne írjon a bufferen kívülre
        return;
    }
    
    // Ellenőrzés, hogy a pixelnek invertáltnak kell-e lennie
    if(SSD1306.Inverted) {
        color = (OLED_COLOR)!color;
    }
    
    // Megfelelő színben rajzolás
    if(color == White)
    {
        OLED_Buffer[x + (y / 8) * OLED_WIDTH] |= 1 << (y % 8);
    }
    else
    {
        OLED_Buffer[x + (y / 8) * OLED_WIDTH] &= ~(1 << (y % 8));
    }
}

// Egy karakter rajzolása a kijelző bufferbe
// ch       => karakter
// Font     => betűtípus
// color    => fekete vagy fehér
char OLED_WriteChar(char ch, FontDef Font, OLED_COLOR color)
{
    uint32_t i, b, j;
    
    // Ellenőrzés, hogy a karakter érvényes-e
    if (ch < 32 || ch > 126)
    {
        return 0;
    }
    
    // Maradék hely ellenőrzése az aktuális sorban
    if (OLED_WIDTH < (SSD1306.CurrentX + Font.FontWidth) ||
        OLED_HEIGHT < (SSD1306.CurrentY + Font.FontHeight))
    {
        // Nincs elég hely az aktuális sorban
        return 0;
    }
    
    // Megadott betűtípus használata az írásra
    for(i = 0; i < Font.FontHeight; i++)
    {
        b = Font.data[(ch - 32) * Font.FontHeight + i];
        for(j = 0; j < Font.FontWidth; j++)
        {
            if((b << j) & 0x8000)
            {
                OLED_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (OLED_COLOR) color);
            }
            else
            {
                OLED_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (OLED_COLOR)!color);
            }
        }
    }
    
    // Az aktuális hely most már be van töltve
    SSD1306.CurrentX += Font.FontWidth;
    
    // Beírt karakter visszaadása validáció miatt
    return ch;
}

// Teljes karakterlánc írása a képernyőbufferbe
char OLED_WriteString(char* str, FontDef Font, OLED_COLOR color)
{

	// Írás null-byte-ig
    while (*str)
    {
        if (OLED_WriteChar(*str, Font, color) != *str)
        {
            // A karaktert nem lehetett kiírni
            return *str;
        }
        
        // Következő karakter
        str++;
    }
    
    // Minden rendben
    return *str;
}

// Kurzor pozíciója
void OLED_SetCursor(uint8_t x, uint8_t y)
{
    SSD1306.CurrentX = x;
    SSD1306.CurrentY = y;
}

// Vonal rajzolása a Bresenham algoritmus alapján
void OLED_Line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, OLED_COLOR color)
{
	int32_t deltaX = abs(x2 - x1);
	int32_t deltaY = abs(y2 - y1);
	int32_t signX = ((x1 < x2) ? 1 : -1);
	int32_t signY = ((y1 < y2) ? 1 : -1);
	int32_t error = deltaX - deltaY;
	int32_t error2;

	OLED_DrawPixel(x2, y2, color);
	while((x1 != x2) || (y1 != y2))
	{
		OLED_DrawPixel(x1, y1, color);
		error2 = error * 2;

		if(error2 > -deltaY)
		{
			error -= deltaY;
			x1 += signX;
		}

		if(error2 < deltaX)
		{
			error += deltaX;
			y1 += signY;
		}
	}

	return;
}

//Vonallánc rajzolás
void OLED_Polyline(const OLED_VERTEX *par_vertex, uint16_t par_size, OLED_COLOR color) {
  uint16_t i;
  if (par_vertex != 0)
  {
    for (i = 1; i < par_size; i++)
    {
    	OLED_Line(par_vertex[i - 1].x, par_vertex[i - 1].y, par_vertex[i].x, par_vertex[i].y, color);
    }
  }
  return;
}

// Fok átváltása radiánba
static float OLED_DegToRad(float par_deg)
{
    return par_deg * 3.14 / 180.0;
}

// Fok normalizálás [0;360] tartományra
static uint16_t OLED_NormalizeTo0_360(uint16_t par_deg)
{
  uint16_t loc_angle;
  if (par_deg <= 360)
  {
    loc_angle = par_deg;
  }
  else
  {
    loc_angle = par_deg % 360;
    loc_angle = ((par_deg != 0)?par_deg:360);
  }
  return loc_angle;
}

/* Körív rajzolás. A körív rajzolása a 4. síknegyedből történik (3 * pi/2)
 * start_angle kezdőfok, fokban megadva
 * sweep hossz, tartomány, fokban megadva
 */
void OLED_DrawArc(uint8_t x, uint8_t y, uint8_t radius, uint16_t start_angle, uint16_t sweep, OLED_COLOR color)
{
    #define CIRCLE_APPROXIMATION_SEGMENTS 36

	float approx_degree;
    uint32_t approx_segments;
    uint8_t xp1,xp2;
    uint8_t yp1,yp2;
    uint32_t count = 0;
    uint32_t loc_sweep = 0;
    float rad;
    
    loc_sweep = OLED_NormalizeTo0_360(sweep);
    
    count = (OLED_NormalizeTo0_360(start_angle) * CIRCLE_APPROXIMATION_SEGMENTS) / 360;

    approx_segments = (loc_sweep * CIRCLE_APPROXIMATION_SEGMENTS) / 360;
    approx_degree = loc_sweep / (float)approx_segments;

    while(count < approx_segments)
    {
        rad = OLED_DegToRad(count*approx_degree);
        xp1 = x + (int8_t)(sin(rad)*radius);
        yp1 = y + (int8_t)(cos(rad)*radius);

        count++;

        if(count != approx_segments)
        {
            rad = OLED_DegToRad(count*approx_degree);
        }
        else
        {            
            rad = OLED_DegToRad(loc_sweep);
        }

        xp2 = x + (int8_t)(sin(rad)*radius);
        yp2 = y + (int8_t)(cos(rad)*radius);

        OLED_Line(xp1,yp1,xp2,yp2,color);
    }
    
    return;
}

// Kör rajzolása a Bresenhem algoritmus alapján
void OLED_DrawCircle(uint8_t par_x,uint8_t par_y,uint8_t par_r,OLED_COLOR par_color)
{
	int32_t x = -par_r;
	int32_t y = 0;
	int32_t err = 2 - 2 * par_r;
	int32_t e2;

	if (par_x >= OLED_WIDTH || par_y >= OLED_HEIGHT)
	{
		return;
	}

	do {
		OLED_DrawPixel(par_x - x, par_y + y, par_color);
		OLED_DrawPixel(par_x + x, par_y + y, par_color);
		OLED_DrawPixel(par_x + x, par_y - y, par_color);
		OLED_DrawPixel(par_x - x, par_y - y, par_color);

		e2 = err;

		if (e2 <= y)
		{
			y++;
			err = err + (y * 2 + 1);

			if (-x == y && e2 <= x)
			{
				e2 = 0;
			}
		}
		if (e2 > x)
		{
			x++;
			err = err + (x * 2 + 1);
		}
	} while (x <= 0);

    return;
}

// Téglalap rajzolása
void OLED_DrawRectangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, OLED_COLOR color)
{
	OLED_Line(x1,y1,x2,y1,color);
	OLED_Line(x2,y1,x2,y2,color);
	OLED_Line(x2,y2,x1,y2,color);
	OLED_Line(x1,y2,x1,y1,color);

	return;
}

void OLED_SetContrast(const uint8_t value)
{
    const uint8_t kSetContrastControlRegister = 0x81;

    OLED_WriteCommand(kSetContrastControlRegister);
    OLED_WriteCommand(value);
}

void OLED_SetDisplayOn(const uint8_t on)
{
    uint8_t value;

    if (on)
    {
        value = 0xAF;   // Kijelző bekapcsolás
        SSD1306.DisplayOn = 1;
    } else {
        value = 0xAE;   // Kijelző kikapcsolás
        SSD1306.DisplayOn = 0;
    }

    OLED_WriteCommand(value);
}

uint8_t OLED_GetDisplayOn()
{
    return SSD1306.DisplayOn;
}
