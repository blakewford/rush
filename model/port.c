#include <avr/io.h>

__attribute__ ((optimize(3)))
void delay(uint32_t milliseconds)
{
    uint32_t cycles = (milliseconds*2280);
    while(cycles--)
    {
        asm volatile("nop");
    }
}
