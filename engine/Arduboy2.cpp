#include <avr/io.h>
#include <avr/pgmspace.h>
#include <Arduboy2.h>

// Original source available here: https://github.com/MLXXXp/Arduboy2 2/6/2019

#ifndef _AVR_ATMEGA4808_H_INCLUDED
#define RAND_SEED_IN_ADMUX (_BV(REFS0) | _BV(REFS1) | _BV(MUX0))
#endif

const uint8_t PROGMEM BootLCD[] =
{
    // Further reading: https://www.adafruit.com/datasheets/SSD1306.pdf
    0xD5, 0xF0,
    0x8D, 0x14,
    0xA1,
    0xC8,
    0x81, 0xCF,
    0xD9, 0xF1,
    0xAF,
    0x20, 0x00,
};

#define RED_LED_BIT      PORTB6
#define GREEN_LED_BIT    PORTB7
#define BLUE_LED_BIT     PORTB5

#define LEFT_BUTTON_BIT  PORTF5
#define RIGHT_BUTTON_BIT PORTF6
#define UP_BUTTON_BIT    PORTF7
#define DOWN_BUTTON_BIT  PORTF4
#define A_BUTTON_BIT     PORTE6
#define B_BUTTON_BIT     PORTB4

#define SPI_MOSI_BIT     PORTB2
#define SPI_SCK_BIT      PORTB1
#define SPI_MISO_BIT     PORTB3
#define SPI_SCK_BIT      PORTB1
#define SPI_SS_BIT       PORTB0

#define RAND_SEED_IN_BIT PORTF1

#define CS_PORT          PORTD
#define CS_BIT           PORTD6
#define DC_PORT          PORTD
#define DC_BIT           PORTD4
#define RST_PORT         PORTD
#define RST_BIT          PORTD7

static uint16_t sFrameCount = 0;
static uint8_t  sBuffer[(HEIGHT*WIDTH)/8];

#define bitSet(value, bit) ((value) |=  (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))

void Arduboy2Base::begin()
{
#ifndef _AVR_ATMEGA4808_H_INCLUDED
    ADMUX  = RAND_SEED_IN_ADMUX;

    PORTB |= _BV(RED_LED_BIT) | _BV(GREEN_LED_BIT) | _BV(BLUE_LED_BIT) | _BV(B_BUTTON_BIT);
    DDRB  &= ~(_BV(B_BUTTON_BIT) | _BV(SPI_MISO_BIT));
    DDRB  |= _BV(RED_LED_BIT) | _BV(GREEN_LED_BIT) | _BV(BLUE_LED_BIT) | _BV(SPI_MOSI_BIT) | _BV(SPI_SCK_BIT) | _BV(SPI_SS_BIT);
    PORTD |= _BV(CS_BIT);
    PORTD &= ~(_BV(RST_BIT));
    DDRD  |= _BV(RST_BIT) | _BV(CS_BIT) | _BV(DC_BIT);
    PORTE |= _BV(A_BUTTON_BIT);
    DDRE  &= ~(_BV(A_BUTTON_BIT));
    PORTF |= _BV(LEFT_BUTTON_BIT) | _BV(RIGHT_BUTTON_BIT) | _BV(UP_BUTTON_BIT) | _BV(DOWN_BUTTON_BIT);
    PORTF &= ~(_BV(RAND_SEED_IN_BIT));
    DDRF  &= ~(_BV(LEFT_BUTTON_BIT) | _BV(RIGHT_BUTTON_BIT) | _BV(UP_BUTTON_BIT) | _BV(DOWN_BUTTON_BIT) | _BV(RAND_SEED_IN_BIT));

    SPCR   = _BV(SPE) | _BV(MSTR);
    SPSR   = _BV(SPI2X);

    delay(5);
    bitSet(RST_PORT, RST_BIT);
    delay(5);
    bitClear(CS_PORT, CS_BIT);
    bitClear(DC_PORT, DC_BIT);

    for(uint8_t i = 0; i < sizeof(BootLCD); i++)
    {
        SPDR = pgm_read_byte(&BootLCD[i]);
        asm volatile("nop");
        while(!(SPSR & _BV(SPIF)))
            ;
    }

    bitSet(DC_PORT, DC_BIT);

    PRR0   = _BV(PRTWI) | _BV(PRADC);
    PRR1  |= _BV(PRUSART1);
#endif
}

void Arduboy2Base::setFrameRate(uint8_t rate)
{
}

bool Arduboy2Base::nextFrame()
{
    sFrameCount++;
    return true;
}

void Arduboy2Base::pollButtons()
{
}

void Arduboy2Base::clear()
{
    uint8_t color = 0;
    uint8_t* bPtr = sBuffer;

    asm volatile
    (
        "cpse %[color], __zero_reg__\n"
        "ldi %[color], 0xFF\n"
        "clr __tmp_reg__\n"
        "loopto:\n"
        "st Z+, %[color]\n"
        "st Z+, %[color]\n"
        "st Z+, %[color]\n"
        "st Z+, %[color]\n"
        "inc __tmp_reg__\n"
        "brne loopto\n"
        : [color] "+d" (color),
          "+z" (bPtr)
        :
        :
    );
}

void Arduboy2Base::display()
{
#ifndef _AVR_ATMEGA4808_H_INCLUDED
    for(int i = 0; i < (HEIGHT*WIDTH)/8; i++)
    {
        SPDR = pgm_read_byte(&sBuffer[i]);
        asm volatile("nop");
        while(!(SPSR & _BV(SPIF)))
            ;
    }
#endif
}

void Arduboy2Base::fillTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint8_t color)
{
/*
// Fill a triangle - Bresenham method
// Original from http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
    uint8_t e1,e2;
    int8_t signx1,signx2,dx1,dy1,dx2,dy2;
    uint8_t t1x,t2x,y,minx,maxx,t1xp,t2xp;

    bool changed1 = false;
    bool changed2 = false;

    if(y1>y2) { std::swap(y1,y2); std::swap(x1,x2); }
    if(y1>y3) { std::swap(y1,y3); std::swap(x1,x3); }
    if(y2>y3) { std::swap(y2,y3); std::swap(x2,x3); }

    t1x=t2x=x1; y=y1;

    dx1 = (int8_t)(x2 - x1); if(dx1<0) { dx1=-dx1; signx1=-1; } else signx1=1;
    dy1 = (int8_t)(y2 - y1);

    dx2 = (int8_t)(x3 - x1); if(dx2<0) { dx2=-dx2; signx2=-1; } else signx2=1;
    dy2 = (int8_t)(y3 - y1);

    if(dy1 > dx1)
    {
        std::swap(dx1,dy1);
        changed1 = true;
    }
    if(dy2 > dx2)
    {
        std::swap(dy2,dx2);
        changed2 = true;
    }

    e2 = (uint8_t)(dx2>>1);

    if(y1 == y2) goto next;
    e1 = (uint8_t)(dx1>>1);

    for(uint8_t i = 0; i < dx1;)
    {
        t1xp=0; t2xp=0;
        if(t1x<t2x)
        {
            minx=t1x; maxx=t2x;
        }
        else
        {
            minx=t2x;
            maxx=t1x;
        }

        while(i<dx1)
        {
            i++;
            e1 += dy1;
            while (e1 >= dx1)
            {
                e1 -= dx1;
                if(changed1) t1xp=signx1;
                else goto next1;
            }
            if(changed1) break;
            else t1x += signx1;
        }
	next1:
        while(true)
        {
            e2 += dy2;
            while(e2 >= dx2)
            {
                e2 -= dx2;
                if(changed2) t2xp=signx2;
                else goto next2;
            }
            if(changed2) break;
            else t2x += signx2;
        }
	next2:
        if(minx>t1x) minx=t1x; if(minx>t2x) minx=t2x;
        if(maxx<t1x) maxx=t1x; if(maxx<t2x) maxx=t2x;
        drawHorizontalLine(minx, maxx, y, color);
        if(!changed1) t1x += signx1;
        t1x+=t1xp;
        if(!changed2) t2x += signx2;
        t2x+=t2xp;

        y += 1;
        if(y == y2) break;
    }
    next:
    dx1 = (int8_t)(x3 - x2); if(dx1<0) { dx1=-dx1; signx1=-1; } else signx1=1;
    dy1 = (int8_t)(y3 - y2);
    t1x=x2;

    if(dy1 > dx1)
    {
        std::swap(dy1,dx1);
        changed1 = true;
    } else changed1=false;

    e1 = (uint8_t)(dx1>>1);

    for(uint8_t i = 0; i<=dx1; i++)
    {
        t1xp=0; t2xp=0;
        if(t1x<t2x)
        {
            minx=t1x;
            maxx=t2x;
        }
        else
        {
            minx=t2x;
            maxx=t1x;
        }
        while(i<dx1)
        {
            e1 += dy1;
            while (e1 >= dx1)
            {
                e1 -= dx1;
                if(changed1)
                {
                    t1xp=signx1; break;
                }
                else goto next3;
            }
            if(changed1) break;
            else t1x += signx1;
            if(i < dx1) i++;
        }
	next3:
        while(t2x != x3)
        {
            e2 += dy2;
            while(e2 >= dx2)
            {
                e2 -= dx2;
                if(changed2) t2xp=signx2;
                else goto next4;
            }
            if(changed2) break;
            else t2x += signx2;
        }
	next4:
        if(minx>t1x) minx=t1x; if(minx>t2x) minx=t2x;
        if(maxx<t1x) maxx=t1x; if(maxx<t2x) maxx=t2x;
        drawHorizontalLine(minx, maxx, y, color);

        if(!changed1) t1x += signx1;
        t1x += t1xp;
        if(!changed2) t2x += signx2;
        t2x+=t2xp;
        y += 1;

        if(y > y3) return;
    }
*/
}

bool Arduboy2Base::everyXFrames(uint8_t frames)
{
    return sFrameCount % frames == 0;
}
