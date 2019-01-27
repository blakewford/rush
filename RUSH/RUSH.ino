#include "assets.h"

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
  arduboy.begin();
  arduboy.setFrameRate(60);
}

int16_t yAngle = 0;

void loop()
{
  if (!(arduboy.nextFrame())) return;
  arduboy.pollButtons();
  arduboy.clear();

  models.drawModel(obj, 15, yAngle, 0, 1);
  if(arduboy.everyXFrames(1))
  {
      yAngle++;
  }

  arduboy.display();
}

