//#include "assets.h"
#ifdef PHASE3
#include "truck.h"
#else
#include "compressed.h"
#endif

#include <Models.h>
#include <Arduino.h>
#include <Arduboy2.h>
#include <ArduboyTones.h>

#include "stdlib.h"
#include "bitmap.h"

Models models;
Sprites sprites;
Arduboy2Base arduboy;
//ArduboyTones sound(arduboy.audio.enabled);

const int16_t START_ANGLE = 180;

uint8_t gScene = 0;
int16_t yAngle = START_ANGLE;

bool decrement = true;
const int8_t MAX_ANGLE = 30;

void setup()
{
#ifndef _AVR_ATMEGA4808_H_INCLUDED
    arduboy.begin();
#endif
    arduboy.setFrameRate(60);
    models.begin();
    arduboy.initRandomSeed();
}

void drawLandscape(int8_t offset)
{
    sprites.drawSelfMasked(offset, 0, ridge_logo, 0);
    sprites.drawSelfMasked(32, 0, logo, 0);
    uint16_t count = 16;

    while(count--)
    {
       long x = random() %  128;
       long y = (random() % 36) + 28;
       arduboy.drawPixel(x, y, 1);
    }
}

int8_t count = 0;
const int8_t HoldCount = 16;

void sizzle()
{

    int8_t offset = 0;
    if(yAngle == START_ANGLE && count != 0)
    {
        count--;
    }
    else if(decrement)
    {
        yAngle--;
    }
    else
    {
        yAngle++;
    }
    if(yAngle > (START_ANGLE + MAX_ANGLE))
    {
        count = HoldCount;
        offset = 1;
        decrement = true;
    }
    if(yAngle < (START_ANGLE-MAX_ANGLE))
    {
        offset = 1;
        count = HoldCount;
        decrement = false;
    }
    drawLandscape(offset);
    models.drawCompressedModel(obj, ndxToValue, 15, yAngle, 0, 1);
}

void loop()
{
    if (!(arduboy.nextFrame())) return;
    arduboy.pollButtons();
    arduboy.clear();

    switch(gScene)
    {
        case 0:
            sizzle();
            break;
        case 1:
//          models.drawModel(obj, 15, yAngle, 0, 1);
            models.drawCompressedModel(obj, ndxToValue, 15, yAngle, 0, 1);
            sprites.drawSelfMasked(3, 16, left, 0);
            sprites.drawSelfMasked(93, 16, right, 0);
            sprites.drawSelfMasked(48, 56, overland, 0);
#ifdef PHASE3
            yAngle+=3;
#else
            yAngle++;
#endif
            break;
    }

    arduboy.display();

    if(arduboy.justPressed(A_BUTTON))
    {
        gScene = 1;
    }
}

