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
    uint8_t* bufferPtr = sBuffer;

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
          "+z" (bufferPtr)
        :
        :
    );
}

void Arduboy2Base::display()
{
#ifndef _AVR_ATMEGA4808_H_INCLUDED
    uint16_t count;
    uint8_t* bufferPtr = sBuffer;

    asm volatile
    (
        "   ldi   %A[count], %[len_lsb]               \n\t"
        "   ldi   %B[count], %[len_msb]               \n\t"
        "1: ld    __tmp_reg__, %a[ptr]      ;2        \n\t"
        "   out   %[spdr], __tmp_reg__      ;1        \n\t"
        "2: sbiw  %A[count], 1              ;2        \n\t"
        "   sbrc  %A[count], 0              ;1/2      \n\t"
        "   rjmp  2b                        ;2        \n\t"
        "   st    %a[ptr]+, __tmp_reg__     ;2        \n\t"
        "   brne  1b                        ;1/2 :18  \n\t"
        "   in    __tmp_reg__, %[spsr]                \n\t"
        : [ptr]     "+&e" (bufferPtr),
          [count]   "=&w" (count)
        : [spdr]    "I"   (_SFR_IO_ADDR(SPDR)),
          [spsr]    "I"   (_SFR_IO_ADDR(SPSR)),
          [len_msb] "M"   (WIDTH * (HEIGHT / 8 * 2) >> 8),
          [len_lsb] "M"   (WIDTH * (HEIGHT / 8 * 2) & 0xFF)
    );
#endif
}

void Arduboy2Base::drawFastHLine(int16_t x, int16_t y, uint8_t w, uint8_t color)
{
    int16_t xEnd;
    if(y < 0 || y >= HEIGHT) return;

    xEnd = x + w;

    if(xEnd <= 0 || x >= WIDTH) return;
    if(x < 0) x = 0;
    if(xEnd > WIDTH) xEnd = WIDTH;
    w = xEnd - x;

    register uint8_t *pBuf = sBuffer + ((y / 8) * WIDTH) + x;
    register uint8_t mask = 1 << (y & 7);

    switch(color)
    {
        case 1:
          while(w--)
          {
              *pBuf++ |= mask;
          }
          break;

        case 0:
          mask = ~mask;
          while(w--)
          {
              *pBuf++ &= mask;
          }
          break;
    }
}

bool Arduboy2Base::everyXFrames(uint8_t frames)
{
    return sFrameCount % frames == 0;
}
