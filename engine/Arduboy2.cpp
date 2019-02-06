#include <avr/io.h>
#include <Arduboy2.h>

#define RAND_SEED_IN_ADMUX (_BV(REFS0) | _BV(REFS1) | _BV(MUX0))

void Arduboy2Base::begin()
{
    ADMUX = RAND_SEED_IN_ADMUX;

    //bootPins();

    SPCR = _BV(SPE) | _BV(MSTR);
    SPSR = _BV(SPI2X);

    //bootOLED();

    PRR0 = _BV(PRTWI) | _BV(PRADC);
    PRR1 |= _BV(PRUSART1);
}

void Arduboy2Base::setFrameRate(uint8_t rate)
{
}

bool Arduboy2Base::nextFrame()
{
}

void Arduboy2Base::pollButtons()
{
}

void Arduboy2Base::clear()
{
}

void Arduboy2Base::display()
{
}

void Arduboy2Base::fillTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint8_t color)
{
}

bool Arduboy2Base::everyXFrames(uint8_t frames)
{
}
