#include <avr/io.h>

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
