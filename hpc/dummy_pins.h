#pragma once

#include <avr/pgmspace.h>

#define DUMMY_REGISTER (0x1084)

#define TIMSK3   (*(register8_t *)DUMMY_REGISTER)
#define OCIE3A   (*(register8_t *)DUMMY_REGISTER)
#define CS31     (*(register8_t *)DUMMY_REGISTER)

#define WGM00    (*(register8_t *)DUMMY_REGISTER)
#define WGM01    (*(register8_t *)DUMMY_REGISTER)
#define WGM10    (*(register8_t *)DUMMY_REGISTER)
#define WGM32    (*(register8_t *)DUMMY_REGISTER)
#define WDE      (*(register8_t *)DUMMY_REGISTER)
#define WDCE     (*(register8_t *)DUMMY_REGISTER)
#define WDTCSR   (*(register8_t *)DUMMY_REGISTER)
#define USBCON   (*(register8_t *)DUMMY_REGISTER)
#define UHWCON   (*(register8_t *)DUMMY_REGISTER)
#define UDINT    (*(register8_t *)DUMMY_REGISTER)
#define UDIEN    (*(register8_t *)DUMMY_REGISTER)
#define UDCON    (*(register8_t *)DUMMY_REGISTER)
#define TCCR0A   (*(register8_t *)DUMMY_REGISTER)
#define TCCR1A   (*(register8_t *)DUMMY_REGISTER)
#define TCCR3A   (*(register8_t *)DUMMY_REGISTER)
#define TCCR3B   (*(register8_t *)DUMMY_REGISTER)

#define SPSR     (*(register8_t *)DUMMY_REGISTER)
#define SPIF     (*(register8_t *)DUMMY_REGISTER)
#define SPI2X    (*(register8_t *)DUMMY_REGISTER)
#define SPE      (*(register8_t *)DUMMY_REGISTER)
#define SPDR     (*(register16_t *)DUMMY_REGISTER)
#define SPCR     (*(register16_t *)DUMMY_REGISTER)
			     
#define SMCR     (*(register8_t *)DUMMY_REGISTER)
#define SE       (*(register8_t *)DUMMY_REGISTER)
			     
#define REFS0    (*(register8_t *)DUMMY_REGISTER)
#define REFS1    (*(register8_t *)DUMMY_REGISTER)

#define DDRB     (*(register8_t *)DUMMY_REGISTER)
#define PINB     (*(register8_t *)DUMMY_REGISTER)

#define DDRC     (*(register8_t *)DUMMY_REGISTER)

#define PORTC6   (*(register8_t *)DUMMY_REGISTER)
#define PORTC7   (*(register8_t *)DUMMY_REGISTER)

#define PORTB0   (*(register8_t *)DUMMY_REGISTER)
#define PORTB1   (*(register8_t *)DUMMY_REGISTER)
#define PORTB2   (*(register8_t *)DUMMY_REGISTER)
#define PORTB3   (*(register8_t *)DUMMY_REGISTER)
#define PORTB4   (*(register8_t *)DUMMY_REGISTER)
#define PORTB5   (*(register8_t *)DUMMY_REGISTER)
#define PORTB6   (*(register8_t *)DUMMY_REGISTER)
#define PORTB7   (*(register8_t *)DUMMY_REGISTER)

#define DDRD     (*(register8_t *)DUMMY_REGISTER)
 
#define PORTD4   (*(register8_t *)DUMMY_REGISTER)
#define PORTD6   (*(register8_t *)DUMMY_REGISTER)
#define PORTD7   (*(register8_t *)DUMMY_REGISTER)
		
#define DDRE     (*(register8_t *)DUMMY_REGISTER)
#define PINE     (*(register8_t *)DUMMY_REGISTER)	     
#define PORTE6   (*(register8_t *)DUMMY_REGISTER)

#define DDRF     (*(register8_t *)DUMMY_REGISTER)			     
#define PINF     (*(register8_t *)DUMMY_REGISTER)
			     
#define PORTF1   (*(register8_t *)DUMMY_REGISTER)
#define PORTF4   (*(register8_t *)DUMMY_REGISTER)
#define PORTF5   (*(register8_t *)DUMMY_REGISTER)
#define PORTF6   (*(register8_t *)DUMMY_REGISTER)
#define PORTF7   (*(register8_t *)DUMMY_REGISTER)
			     
#define ADSC     (*(register8_t *)DUMMY_REGISTER)
#define ADMUX    (*(register8_t *)DUMMY_REGISTER)
#define ADCSRA   (*(register8_t *)DUMMY_REGISTER)
#define ADC      (*(register8_t *)DUMMY_REGISTER)
			     
#define PRR0     (*(register8_t *)DUMMY_REGISTER)
#define PRR1     (*(register8_t *)DUMMY_REGISTER)
				 
#define PRUSART1 (*(register8_t *)DUMMY_REGISTER)
#define PSRTWI   (*(register8_t *)DUMMY_REGISTER)
#define PRADC    (*(register8_t *)DUMMY_REGISTER)
#define PRTWI    (*(register8_t *)DUMMY_REGISTER)

#define OCR0A    (*(register8_t *)DUMMY_REGISTER)
#define OCR3A    (*(register8_t *)DUMMY_REGISTER)
#define OCR1AL   (*(register8_t *)DUMMY_REGISTER)
#define OCR1BL   (*(register8_t *)DUMMY_REGISTER)

#define COM0A0   (*(register8_t *)DUMMY_REGISTER)
#define COM0A1   (*(register8_t *)DUMMY_REGISTER)
#define COM0B0   (*(register8_t *)DUMMY_REGISTER)
#define COM0B1   (*(register8_t *)DUMMY_REGISTER)

#define COM1A0   (*(register8_t *)DUMMY_REGISTER)
#define COM1A1   (*(register8_t *)DUMMY_REGISTER)
#define COM1B0   (*(register8_t *)DUMMY_REGISTER)
#define COM1B1   (*(register8_t *)DUMMY_REGISTER)

#define MUX0     (*(register8_t *)DUMMY_REGISTER)
#define MSTR     (*(register8_t *)DUMMY_REGISTER)

#define FRZCLK   (*(register8_t *)DUMMY_REGISTER)
#define DETACH   (*(register8_t *)DUMMY_REGISTER)

#undef PORTB
#define PORTB    (*(register8_t *)DUMMY_REGISTER)

#undef PORTD
#define PORTD    (*(register8_t *)DUMMY_REGISTER)

#undef PORTE
#define PORTE    (*(register8_t *)DUMMY_REGISTER)

#undef PORTF
#define PORTF    (*(register8_t *)DUMMY_REGISTER)

#define power_usb_enable()
#define power_usb_disable() 

#define power_timer0_disable()

#define power_adc_enable()
#define power_adc_disable()

// Included here for 4808 convenience
void fillTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint8_t color);