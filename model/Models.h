#ifndef Models_h
#define Models_h

#include "Arduboy2.h"

class Models
{
public:
    static void drawModel(const float* model, int16_t xAngle, int16_t yAngle, int16_t zAngle, uint8_t color);
    static void drawCompressedModel(const uint8_t* model, const float* map, int16_t xAngle, int16_t yAngle, int16_t zAngle, uint8_t color);

private:
    static void drawModel(int16_t xAngle, int16_t yAngle, int16_t zAngle, uint8_t color);
};

#endif
