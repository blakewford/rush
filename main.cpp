#include <Arduboy2.h>

extern Arduboy2Base arduboy;

#ifndef __AVR__
int32_t SDL_Init();
extern bool gKeepGoing;
void* RenderThread(void* buffer);
#else
#include "RUSH/RUSH.ino"
const bool gKeepGoing = true;
#endif

#include "model/Models.cpp"

void setup();
void loop();

void rush()
{
    arduboy.clear();
#ifndef __AVR__
    if(SDL_Init() < 0) return;
    uint32_t texture[WIDTH*HEIGHT];
#endif
    setup();
    while(gKeepGoing)
    {
        loop();
#ifndef __AVR__
        RenderThread(texture);
#endif
    }
}

int main()
{
    rush();
    return 0;
}
