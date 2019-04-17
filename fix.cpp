#ifndef __AVR__
#include <algorithm>
int32_t SDL_Init();
extern bool gKeepGoing;
void* RenderThread(void* buffer);
uint32_t gTexture[WIDTH*HEIGHT];
extern Arduboy2Base arduboy;

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DEBUG_PORT 8080
#else
#include "RUSH/RUSH.ino"
bool gKeepGoing = true;
#include <avr/interrupt.h>
#endif

uint16_t gReportedVerts = 0;

void init()
{
#ifdef __AVR__
// Derived from source here https://github.com/arduino/ArduinoCore-avr 2/9/2019
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

#ifdef __AVR__
namespace std
{
    template< class T >
    void swap(T& A, T& B)
    {
        T C = A;
        A = B;
        B = C;
    }
}

void Sprites::drawSelfMasked(int x, int y, unsigned char const* bitmap, unsigned char)
{
    uint8_t width = pgm_read_byte(bitmap++);
    uint8_t height = pgm_read_byte(bitmap);
    drawBitmap(x, y, bitmap, bitmap, width, height, SPRITE_IS_MASK);
}

#endif

void Arduboy2Audio::begin()
{
}

void fillTriangle(int16_t x1, int16_t y1, int16_t x2, int16_t y2, int16_t x3, int16_t y3, uint8_t color)
{
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
        arduboy.drawFastHLine(minx, y, maxx-minx, color);
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
        arduboy.drawFastHLine(minx, y, maxx-minx, color);

        if(!changed1) t1x += signx1;
        t1x += t1xp;
        if(!changed2) t2x += signx2;
        t2x+=t2xp;
        y += 1;

        if(y > y3) return;
    }
}

#ifdef __AVR__
//Pulled from library https://github.com/MLXXXp/Arduboy2/blob/master/src/Sprites.cpp
void Sprites::drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, const uint8_t *mask, uint8_t w, uint8_t h, uint8_t draw_mode)
{
  // no need to draw at all of we're offscreen
  if (x + w <= 0 || x > WIDTH - 1 || y + h <= 0 || y > HEIGHT - 1)
    return;

  if (bitmap == NULL)
    return;

  // xOffset technically doesn't need to be 16 bit but the math operations
  // are measurably faster if it is
  uint16_t xOffset, ofs;
  int8_t yOffset = y & 7;
  int8_t sRow = y / 8;
  uint8_t loop_h, start_h, rendered_width;

  if (y < 0 && yOffset > 0) {
    sRow--;
  }

  // if the left side of the render is offscreen skip those loops
  if (x < 0) {
    xOffset = abs(x);
  } else {
    xOffset = 0;
  }

  // if the right side of the render is offscreen skip those loops
  if (x + w > WIDTH - 1) {
    rendered_width = ((WIDTH - x) - xOffset);
  } else {
    rendered_width = (w - xOffset);
  }

  // if the top side of the render is offscreen skip those loops
  if (sRow < -1) {
    start_h = abs(sRow) - 1;
  } else {
    start_h = 0;
  }

  loop_h = h / 8 + (h % 8 > 0 ? 1 : 0); // divide, then round up

  // if (sRow + loop_h - 1 > (HEIGHT/8)-1)
  if (sRow + loop_h > (HEIGHT / 8)) {
    loop_h = (HEIGHT / 8) - sRow;
  }

  // prepare variables for loops later so we can compare with 0
  // instead of comparing two variables
  loop_h -= start_h;

  sRow += start_h;
  ofs = (sRow * WIDTH) + x + xOffset;
  uint8_t *bofs = (uint8_t *)bitmap + (start_h * w) + xOffset;
  uint8_t data;

  uint8_t mul_amt = 1 << yOffset;
  uint16_t mask_data;
  uint16_t bitmap_data;

  switch (draw_mode) {
    case SPRITE_UNMASKED:
      // we only want to mask the 8 bits of our own sprite, so we can
      // calculate the mask before the start of the loop
      mask_data = ~(0xFF * mul_amt);
      // really if yOffset = 0 you have a faster case here that could be
      // optimized
      for (uint8_t a = 0; a < loop_h; a++) {
        for (uint8_t iCol = 0; iCol < rendered_width; iCol++) {
          bitmap_data = pgm_read_byte(bofs) * mul_amt;

          if (sRow >= 0) {
            data = Arduboy2Base::sBuffer[ofs];
            data &= (uint8_t)(mask_data);
            data |= (uint8_t)(bitmap_data);
            Arduboy2Base::sBuffer[ofs] = data;
          }
          if (yOffset != 0 && sRow < 7) {
            data = Arduboy2Base::sBuffer[ofs + WIDTH];
            data &= (*((unsigned char *) (&mask_data) + 1));
            data |= (*((unsigned char *) (&bitmap_data) + 1));
            Arduboy2Base::sBuffer[ofs + WIDTH] = data;
          }
          ofs++;
          bofs++;
        }
        sRow++;
        bofs += w - rendered_width;
        ofs += WIDTH - rendered_width;
      }
      break;

    case SPRITE_IS_MASK:
      for (uint8_t a = 0; a < loop_h; a++) {
        for (uint8_t iCol = 0; iCol < rendered_width; iCol++) {
          bitmap_data = pgm_read_byte(bofs) * mul_amt;
          if (sRow >= 0) {
            Arduboy2Base::sBuffer[ofs] |= (uint8_t)(bitmap_data);
          }
          if (yOffset != 0 && sRow < 7) {
            Arduboy2Base::sBuffer[ofs + WIDTH] |= (*((unsigned char *) (&bitmap_data) + 1));
          }
          ofs++;
          bofs++;
        }
        sRow++;
        bofs += w - rendered_width;
        ofs += WIDTH - rendered_width;
      }
      break;

    case SPRITE_IS_MASK_ERASE:
      for (uint8_t a = 0; a < loop_h; a++) {
        for (uint8_t iCol = 0; iCol < rendered_width; iCol++) {
          bitmap_data = pgm_read_byte(bofs) * mul_amt;
          if (sRow >= 0) {
            Arduboy2Base::sBuffer[ofs]  &= ~(uint8_t)(bitmap_data);
          }
          if (yOffset != 0 && sRow < 7) {
            Arduboy2Base::sBuffer[ofs + WIDTH] &= ~(*((unsigned char *) (&bitmap_data) + 1));
          }
          ofs++;
          bofs++;
        }
        sRow++;
        bofs += w - rendered_width;
        ofs += WIDTH - rendered_width;
      }
      break;

    case SPRITE_MASKED:
      uint8_t *mask_ofs;
      mask_ofs = (uint8_t *)mask + (start_h * w) + xOffset;
      for (uint8_t a = 0; a < loop_h; a++) {
        for (uint8_t iCol = 0; iCol < rendered_width; iCol++) {
          // NOTE: you might think in the yOffset==0 case that this results
          // in more effort, but in all my testing the compiler was forcing
          // 16-bit math to happen here anyways, so this isn't actually
          // compiling to more code than it otherwise would. If the offset
          // is 0 the high part of the word will just never be used.

          // load data and bit shift
          // mask needs to be bit flipped
          mask_data = ~(pgm_read_byte(mask_ofs) * mul_amt);
          bitmap_data = pgm_read_byte(bofs) * mul_amt;

          if (sRow >= 0) {
            data = Arduboy2Base::sBuffer[ofs];
            data &= (uint8_t)(mask_data);
            data |= (uint8_t)(bitmap_data);
            Arduboy2Base::sBuffer[ofs] = data;
          }
          if (yOffset != 0 && sRow < 7) {
            data = Arduboy2Base::sBuffer[ofs + WIDTH];
            data &= (*((unsigned char *) (&mask_data) + 1));
            data |= (*((unsigned char *) (&bitmap_data) + 1));
            Arduboy2Base::sBuffer[ofs + WIDTH] = data;
          }
          ofs++;
          mask_ofs++;
          bofs++;
        }
        sRow++;
        bofs += w - rendered_width;
        mask_ofs += w - rendered_width;
        ofs += WIDTH - rendered_width;
      }
      break;


    case SPRITE_PLUS_MASK:
      // *2 because we use double the bits (mask + bitmap)
      bofs = (uint8_t *)(bitmap + ((start_h * w) + xOffset) * 2);

      uint8_t xi = rendered_width; // counter for x loop below

      asm volatile(
        "push r28\n" // save Y
        "push r29\n"
        "movw r28, %[buffer_ofs]\n" // Y = buffer_ofs_2
        "adiw r28, 63\n" // buffer_ofs_2 = buffer_ofs + 128
        "adiw r28, 63\n"
        "adiw r28, 2\n"
        "loop_y:\n"
        "loop_x:\n"
        // load bitmap and mask data
        "lpm %A[bitmap_data], Z+\n"
        "lpm %A[mask_data], Z+\n"

        // shift mask and buffer data
        "tst %[yOffset]\n"
        "breq skip_shifting\n"
        "mul %A[bitmap_data], %[mul_amt]\n"
        "movw %[bitmap_data], r0\n"
        "mul %A[mask_data], %[mul_amt]\n"
        "movw %[mask_data], r0\n"

        // SECOND PAGE
        // if yOffset != 0 && sRow < 7
        "cpi %[sRow], 7\n"
        "brge end_second_page\n"
        // then
        "ld %[data], Y\n"
        "com %B[mask_data]\n" // invert high byte of mask
        "and %[data], %B[mask_data]\n"
        "or %[data], %B[bitmap_data]\n"
        // update buffer, increment
        "st Y+, %[data]\n"

        "end_second_page:\n"
        "skip_shifting:\n"

        // FIRST PAGE
        // if sRow >= 0
        "tst %[sRow]\n"
        "brmi skip_first_page\n"
        "ld %[data], %a[buffer_ofs]\n"
        // then
        "com %A[mask_data]\n"
        "and %[data], %A[mask_data]\n"
        "or %[data], %A[bitmap_data]\n"
        // update buffer, increment
        "st %a[buffer_ofs]+, %[data]\n"
        "jmp end_first_page\n"

        "skip_first_page:\n"
        // since no ST Z+ when skipped we need to do this manually
        "adiw %[buffer_ofs], 1\n"

        "end_first_page:\n"

        // "x_loop_next:\n"
        "dec %[xi]\n"
        "brne loop_x\n"

        // increment y
        "next_loop_y:\n"
        "dec %[yi]\n"
        "breq finished\n"
        "mov %[xi], %[x_count]\n" // reset x counter
        // sRow++;
        "inc %[sRow]\n"
        "clr __zero_reg__\n"
        // sprite_ofs += (w - rendered_width) * 2;
        "add %A[sprite_ofs], %A[sprite_ofs_jump]\n"
        "adc %B[sprite_ofs], __zero_reg__\n"
        // buffer_ofs += WIDTH - rendered_width;
        "add %A[buffer_ofs], %A[buffer_ofs_jump]\n"
        "adc %B[buffer_ofs], __zero_reg__\n"
        // buffer_ofs_page_2 += WIDTH - rendered_width;
        "add r28, %A[buffer_ofs_jump]\n"
        "adc r29, __zero_reg__\n"

        "rjmp loop_y\n"
        "finished:\n"
        // put the Y register back in place
        "pop r29\n"
        "pop r28\n"
        "clr __zero_reg__\n" // just in case
        : [xi] "+&a" (xi),
        [yi] "+&a" (loop_h),
        [sRow] "+&a" (sRow), // CPI requires an upper register (r16-r23)
        [data] "=&l" (data),
        [mask_data] "=&l" (mask_data),
        [bitmap_data] "=&l" (bitmap_data)
        :
        [screen_width] "M" (WIDTH),
        [x_count] "l" (rendered_width), // lower register
        [sprite_ofs] "z" (bofs),
        [buffer_ofs] "x" (Arduboy2Base::sBuffer+ofs),
        [buffer_ofs_jump] "a" (WIDTH-rendered_width), // upper reg (r16-r23)
        [sprite_ofs_jump] "a" ((w-rendered_width)*2), // upper reg (r16-r23)

        // [sprite_ofs_jump] "r" (0),
        [yOffset] "l" (yOffset), // lower register
        [mul_amt] "l" (mul_amt) // lower register
        // NOTE: We also clobber r28 and r29 (y) but sometimes the compiler
        // won't allow us, so in order to make this work we don't tell it
        // that we clobber them. Instead, we push/pop to preserve them.
        // Then we need to guarantee that the the compiler doesn't put one of
        // our own variables into r28/r29.
        // We do that by specifying all the inputs and outputs use either
        // lower registers (l) or simple (r16-r23) upper registers (a).
        : // pushes/clobbers/pops r28 and r29 (y)
      );
      break;
  }
}
#endif