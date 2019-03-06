#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

typedef int ssize_t;
namespace std
{
    typedef unsigned int size_t;
    typedef int ptrdiff_t;
}

#define CHAR_BIT 8
#define boolean bool

#define B00000000  0x0
#define B00000001  0x1
#define B00000010  0x2
#define B00000100  0x4
#define B00001000  0x8
#define B00001010  0xA
#define B00100000 0x20