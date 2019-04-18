//#include "assets.h"
#include "bus.h"
#include "bike.h"
#include "truck.h"
#include "compressed.h"

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
#ifdef __AVR__
    arduboy.setFrameRate(60);
#else
    arduboy.setFrameRate(24);
#endif
    models.begin();
    arduboy.initRandomSeed();
}

long pixelX[16];
long pixelY[16];
void drawLandscape(int8_t offset, bool branded)
{
    sprites.drawSelfMasked(offset, 0, branded ? ridge_logo: ridge, 0);
    if(branded)
    {
        sprites.drawSelfMasked(32, 0, logo, 0);
    }

    uint16_t count = 16;
#ifndef __AVR__
    if(arduboy.everyXFrames(7))
#endif
    {
        while(count--)
        {
           pixelX[count] = random() %  128;
           pixelY[count] = (random() % 36) + 28;
        }
    }
    count = 16;
    while(count--)
    {
        arduboy.drawPixel(pixelX[count], pixelY[count], 1);
    }
}

int8_t count = 0;
#ifdef __AVR__
const int8_t HoldCount = 2;
#else
const int8_t HoldCount = 16;
#endif

void sizzle()
{
    int8_t offset = 0;
    if(yAngle == START_ANGLE && count != 0)
    {
        count--;
    }
    else if(decrement)
    {
#ifdef __AVR__
        yAngle-=7;
#else
        yAngle--;
#endif
    }
    else
    {
#ifdef __AVR__
        yAngle+=7;
#else
        yAngle++;
#endif
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
    drawLandscape(offset, true);
    models.drawCompressedModel(car, ndxToValueCar, 15, yAngle, 0, 1);

    if(arduboy.justPressed(A_BUTTON))
    {
        gScene = 1;
    }
}

uint32_t gCount = 0;
int8_t gSelection = 0;

void game()
{
    float* modelMap = nullptr;
    uint8_t* vehicle = nullptr;
    switch(gSelection)
    {
        case 0:
            vehicle = car;
            modelMap = ndxToValueCar;
            break;
        case 1:
            vehicle = truck;
            modelMap = ndxToValueTruck;
            break;
        case 2:
            vehicle = bus;
            modelMap = ndxToValueBus;
            break;
        case 3:
            vehicle = bike;
            modelMap = ndxToValueBike;
            break;
    }
    if(gCount < 72)
    {
        drawLandscape(0, false);
        models.drawCompressedModel(vehicle, modelMap, 15, START_ANGLE+MAX_ANGLE, 0, 1);
        gCount++;
    }
    else if(gCount >= 72 && gCount < 92)
    {
        drawLandscape(0, false);
        models.drawCompressedModel(vehicle, modelMap, random()%15, START_ANGLE+random()%180, 0, 1);
        gCount++;
        delay(200);
    }
    else
    {
        sprites.drawSelfMasked(0, 0, ridge, 0);

        long smokeX = 0;
        long smokeY = 0;
        uint16_t count = 16;
        while(count--)
        {
            arduboy.drawPixel(pixelX[count], pixelY[count], 1);
            smokeX = (random() % 12) + 58;
            smokeY = random() %  28;
            arduboy.drawPixel(smokeX, smokeY, 1);
        }
        models.drawCompressedModel(vehicle, modelMap, 180, START_ANGLE+MAX_ANGLE, 0, 1);
    }
}

int8_t last = 1;
void selection()
{
    float* modelMap = nullptr;
    uint8_t* vehicle = nullptr;
    uint8_t* name = nullptr;
    switch(gSelection)
    {
        case 0:
            vehicle = car;
            modelMap = ndxToValueCar;
            name = overland;
            break;
        case 1:
            vehicle = truck;
            modelMap = ndxToValueTruck;
            name = baja;
            break;
        case 2:
            vehicle = bus;
            modelMap = ndxToValueBus;
            name = burningman;
            break;
        case 3:
            vehicle = bike;
            modelMap = ndxToValueBike;
            name = moto;
            break;
    }

//    models.drawModel(obj, 15, yAngle, 0, 1);
    models.drawCompressedModel(vehicle, modelMap, 15, yAngle, 0, 1);
    sprites.drawSelfMasked(3, 16, left, 0);
    sprites.drawSelfMasked(93, 16, right, 0);
    sprites.drawSelfMasked(43, 56, name, 0);
#ifdef __AVR__
    switch(gSelection)
    {
        case 0:
            yAngle+=7;
            break;
        case 1:
            yAngle+=5;
            break;
        case 2:
            yAngle+=4;
            break;
        case 3:
            yAngle+=last;
            last++;
            if(last > 4)
                last = 1;
            break;
    }
#else
    yAngle++;
#endif

    if(arduboy.justPressed(LEFT_BUTTON))
    {
        gSelection--;
    }

    if(arduboy.justPressed(RIGHT_BUTTON))
    {
        gSelection++;
    }

    if(arduboy.justPressed(A_BUTTON))
    {
        gScene = 2;
    }

    if(gSelection < 0)
    {
        gSelection = 3;
    }

    if(gSelection > 3)
    {
        gSelection = 0;
    }
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
            selection();
            break;
        case 2:
            game();
            break;
    }

    arduboy.display();
}

