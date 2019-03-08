#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "pins_arduino.h"

typedef int ssize_t;
namespace std
{
    typedef unsigned int size_t;
    typedef int ptrdiff_t;
}

#define CHAR_BIT 8
typedef bool boolean;

extern "C"
{
    void pinMode(uint8_t, uint8_t);
    void digitalWrite(uint8_t, uint8_t);
}

#define noInterrupts() cli()
#define digitalPinToBitMask(P) 0
#define digitalPinToPort(P) 0
#define portOutputRegister(P) 0
#define OUTPUT 0x1

