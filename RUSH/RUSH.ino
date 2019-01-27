#include <Models.h>
#include <Arduino.h>
#include <Arduboy2.h>
#include <ArduboyTones.h>

Sprites sprites;
Arduboy2Base arduboy;
//ArduboyTones sound(arduboy.audio.enabled);

void setup()
{
  arduboy.begin();
  arduboy.setFrameRate(60);
}

void loop()
{
  if (!(arduboy.nextFrame())) return;
  arduboy.pollButtons();
  arduboy.clear();

  // Game

  arduboy.display();
}

