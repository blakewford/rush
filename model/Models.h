#ifndef Models_h
#define Models_h

#include "Arduboy2.h"

const uint8_t BUFFER_SIZE = 16;

struct param
{
    float value[BUFFER_SIZE];
    float shape[BUFFER_SIZE];
    char name[BUFFER_SIZE];

    param()
    {
        memset(value, '\0', sizeof(float)*BUFFER_SIZE);
        memset(shape, '\0', sizeof(float)*BUFFER_SIZE);
    }
};

class Models
{
public:
    static void begin();
    static void drawModel(const float* model, int16_t xAngle, int16_t yAngle, int16_t zAngle, uint8_t color);
    static void drawCompressedModel(const uint8_t* model, const float* map, int16_t xAngle, int16_t yAngle, int16_t zAngle, uint8_t color);

private:

    static param s_Ortho;
    static param s_zAngle;
    static void drawModel(int16_t xAngle, int16_t yAngle, int16_t zAngle, uint8_t color);
};

#endif
