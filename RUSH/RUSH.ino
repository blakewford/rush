//#include "assets.h"
#include "compressed.h"

#include <Models.h>
#include <Arduino.h>
#include <Arduboy2.h>
#include <ArduboyTones.h>

Models models;
Sprites sprites;
Arduboy2Base arduboy;
//ArduboyTones sound(arduboy.audio.enabled);

void setup()
{
#ifndef _AVR_ATMEGA4808_H_INCLUDED
    arduboy.begin();
#endif
    arduboy.setFrameRate(60);
    models.begin();
}

int16_t yAngle = 0;

void loop()
{
    if (!(arduboy.nextFrame())) return;
    arduboy.pollButtons();
    arduboy.clear();

//    models.drawModel(obj, 15, yAngle, 0, 1);
    models.drawCompressedModel(obj, ndxToValue, 15, yAngle, 0, 1);
    yAngle++;

    arduboy.display();
}

