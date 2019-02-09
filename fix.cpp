#ifndef __AVR__
int32_t SDL_Init();
extern bool gKeepGoing;
void* RenderThread(void* buffer);
uint32_t gTexture[WIDTH*HEIGHT];
#else
#include "RUSH/RUSH.ino"
const bool gKeepGoing = true;
#include <avr/interrupt.h>

#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void init()
{
#ifdef __AVR__
#ifndef _AVR_ATMEGA4808_H_INCLUDED
    sei();

    sbi(TCCR0A, WGM01);
    sbi(TCCR0A, WGM00);
    sbi(TCCR0B, CS01);
    sbi(TCCR0B, CS00);
    TCCR1B = 0;
    sbi(TCCR1B, CS11);
    sbi(TCCR1B, CS10);
    sbi(TCCR1A, WGM10);
    sbi(TCCR3B, CS31);
    sbi(TCCR3B, CS30);
    sbi(TCCR3A, WGM30);
    sbi(TCCR4B, CS42);
    sbi(TCCR4B, CS41);
    sbi(TCCR4B, CS40);
    sbi(TCCR4D, WGM40);
    sbi(TCCR4A, PWM4A);
    sbi(TCCR4C, PWM4D);
    sbi(ADCSRA, ADPS2);
    sbi(ADCSRA, ADPS1);
    sbi(ADCSRA, ADPS0);
    cbi(ADCSRA, ADPS2);
    cbi(ADCSRA, ADPS1);
    sbi(ADCSRA, ADPS0);
    sbi(ADCSRA, ADEN);
#endif
#endif

#ifndef __AVR__
    if(SDL_Init() < 0) return;
#endif
}

void post()
{
#ifndef __AVR__
        RenderThread(gTexture);
#endif
}

inline void setup();
inline void loop();