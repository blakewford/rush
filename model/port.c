#include <avr/io.h>

// Derived from source here https://github.com/arduino/ArduinoCore-avr 2/9/2019

__attribute__ ((optimize(3)))
void delay(uint32_t milliseconds)
{
    uint16_t us = (uint16_t)(milliseconds*1000);
    if(us <= 1) return;
#ifdef _AVR_ATMEGA4808_H_INCLUDED
    __asm__ __volatile__
    (
        "nop" "\n\t"
        "nop" "\n\t"
        "nop" "\n\t"
        "nop"
    );
    us = (us << 2) + us;
    us -= 7;
#else
    us <<= 2;
    us -= 5;
#endif

    __asm__ __volatile__
    (
        "1: sbiw %0,1" "\n\t"
        "brne 1b" : "=w" (us) : "0" (us)
    );
}

#ifndef _AVR_ATMEGA4808_H_INCLUDED
ssize_t write(int fd, const void *buf, size_t count)
{
    char* c = (const char*)buf;
    while(count--)
    {
        while ( !( UCSR1A & (1<<UDRE1)) )
            ;
        UDR1 = *c;
        c++;
    }

    return 0;
}
#endif