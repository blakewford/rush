#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#include "Models.h"

extern Arduboy2Base arduboy;

//#define PROFILE

/*
namespace std
{
    void swap(long long& A, long long& B)
    {
        long long C = A;
        A = B;
        B = C;
    }
}
*/

#include "runtime_single_threaded_matmul.cc"

enum parse_state: int8_t
{
    CONSTANTS,
    SHAPE,
    NAME,
    UNKNOWN
};

void TensorPort(const param& A, const param& B, float* C)
{
    assert(A.shape[1] == B.shape[0]);
//    MatMul(NULL, C, (float*)A.value, (float*)B.value, A.shape[0], B.shape[1], A.shape[1], true, true);
    MatMul(C, (float*)A.value, (float*)B.value, A.shape[0]);
}

void rotationEntry(const int16_t angle, param& parameter, rotation_axis axis)
{
    const float radians = (angle%360)*0.0174533;

//    parameter.shape[0] = 3;
    parameter.shape[1] = 3;

    const float sine = sin(radians);
    const float cosine = cos(radians);

    switch(axis)
    {
        case X:
            parameter.value[0] = 1;
/*
            parameter.value[1] = 0;
            parameter.value[2] = 0;
            parameter.value[3] = 0;
*/
            parameter.value[4] = cosine;
            parameter.value[5] = sine;
//            parameter.value[6] = 0;
            parameter.value[7] = -sine;
            parameter.value[8] = cosine;
            break;
        case Y:
            parameter.value[0] = cosine;
//            parameter.value[1] = 0;
            parameter.value[2] = -sine;
//            parameter.value[3] = 0;
            parameter.value[4] = 1;
//            parameter.value[5] = 0;
            parameter.value[6] = sine;
//            parameter.value[7] = 0;
            parameter.value[8] = cosine;
            break;
/*
        case Z:
            parameter.value[0] = cos(radians);
            parameter.value[1] = sin(radians);
            parameter.value[2] = 0;
            parameter.value[3] = -sin(radians);
            parameter.value[4] = cos(radians);
            parameter.value[5] = 0;
            parameter.value[6] = 0;
            parameter.value[7] = 0;
            parameter.value[8] = 1;
            break;
*/
        default:
            break;
    }
}
/*
void parseEntry(const char* cursor, param& parameter, int32_t& valueSize, int32_t& shapeSize)
{
    valueSize = 0;
    shapeSize = 0;

    int32_t i = 0;
    bool nameOpen = false;
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);

    parse_state state = UNKNOWN;

    while(*cursor != '\0')
    {
        char current = *cursor;
        switch(current)
        {
            case '[':
                if(state == UNKNOWN)
                {
                    state = CONSTANTS; // Cannot have non-keyword args after keyword args, so default to non-keyword.
                }
                break;
            case ',':
                switch(state)
                {
                    case CONSTANTS:
                        parameter.value[valueSize++] = strtod(buffer, NULL);
                        break;
                    case SHAPE:
                        parameter.shape[shapeSize++] = strtod(buffer, NULL);
                        break;
                }
                memset(buffer, '\0', BUFFER_SIZE);
                i = 0;
                break;
            case '=':
                {
                    char* front = buffer;
                    while(isspace(*front))
                        front++;

                    if(!strcmp(front, "shape"))
                    {
                        state = SHAPE;
                    }
                    else if(!strcmp(front, "name"))
                    {
                        state = NAME;
                    }
                    memset(buffer, '\0', BUFFER_SIZE);
                    i = 0;
                }
                break;
            case '\'':
                if(state == NAME && !nameOpen)
                {
                    nameOpen = true;
                }
                else
                {
                    strcpy(parameter.name, buffer);
                    memset(buffer, '\0', BUFFER_SIZE);
                    i = 0;
                    nameOpen = false;
                }
                break;
            case '.':
                if(state == CONSTANTS)
                {
                    buffer[i++] = current;
                }
                break;
            case ']':
                switch(state)
                {
                    case CONSTANTS:
                        parameter.value[valueSize++] = strtod(buffer, NULL);
                        break;
                    case SHAPE:
                        parameter.shape[shapeSize++] = strtod(buffer, NULL);
                        break;
                    default:
                        assert(0);
                }
                memset(buffer, '\0', BUFFER_SIZE);
                i = 0;
                state = UNKNOWN;
                break;
            default:
                if(state == CONSTANTS || state == SHAPE)
                {
                    if(isdigit(current) || current == '-')
                    {
                        buffer[i++] = current;
                    }
                }
                else if(state == UNKNOWN || state == NAME)
                {
                    buffer[i++] = current; //Parsing the name of the state
                }
                break;
        }
        cursor++;
    }
}
*/

float ortho[4][4] =
{
    {1.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 1.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 1.0f},
};

struct vertex
{
    float x;
    float y;
    float z;
};

float copy[40];

param Models::s_Ortho;
param Models::s_zAngle;

void Models::begin()
{
//    rotationEntry(zAngle, s_zAngle, Z);
    s_zAngle.value[0] = 1;
//    s_zAngle.value[1] = 0;
//    s_zAngle.value[2] = 0;
//    s_zAngle.value[3] = -0;
    s_zAngle.value[4] = 1;
//    s_zAngle.value[5] = 0;
//    s_zAngle.value[6] = 0;
//    s_zAngle.value[7] = 0;
    s_zAngle.value[8] = 1;
 //   s_zAngle.shape[0] = 3;
    s_zAngle.shape[1] = 3;

    s_Ortho.shape[0] = 4;
    s_Ortho.shape[1] = 4;
    memcpy(s_Ortho.value, ortho, 16*sizeof(float));
}

void Models::drawModel(const float* model, int16_t xAngle, int16_t yAngle, int16_t zAngle, uint8_t color)
{
#ifdef PROFILE
    microseconds start = duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch());
#endif
    int16_t count = (int16_t)model[0];
    count*=3;
    count++;
    memcpy(copy, model, count*sizeof(float));
    drawModel(xAngle, yAngle, zAngle, color);

#ifdef PROFILE
    microseconds end = duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch());
    printf("Microseconds %lld\n", end.count()-start.count());
#endif
}

void Models::drawCompressedModel(const uint8_t* model, const float* map, int16_t xAngle, int16_t yAngle, int16_t zAngle, uint8_t color)
{
#ifdef PROFILE
    microseconds start = duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch());
#endif
    int16_t count = (int16_t)map[0];
    count*=3;

    copy[0] = 3;
    int16_t ndx = 0;
    while(ndx < count)
    {
        copy[1] = map[model[ndx]];
        copy[2] = map[model[ndx+1]];
        copy[3] = map[model[ndx+2]];
        copy[4] = map[model[ndx+3]];
        copy[5] = map[model[ndx+4]];
        copy[6] = map[model[ndx+5]];
        copy[7] = map[model[ndx+6]];
        copy[8] = map[model[ndx+7]];
        copy[9] = map[model[ndx+8]];
        drawModel(xAngle, yAngle, zAngle, color);
        ndx+=9;
    }

#ifdef PROFILE
    microseconds end = duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch());
    printf("Microseconds %lld\n", end.count()-start.count());
#endif
}

void Models::modifyAngle(const int16_t angle, const rotation_axis axis)
{
    param A, B;
    int16_t current = 1;
    int16_t count = (int16_t)copy[0];


    rotationEntry(angle, A, axis);
    while(count--)
    {
        int16_t start = current;
        B.value[2] = copy[current++];
        B.value[1] = copy[current++];
        B.value[0] = copy[current++];
//        B.shape[0] = 3;
//        B.shape[1] = 1;
        float C[(int8_t)(A.shape[0]*B.shape[1])];
        TensorPort(A, B, C);
        memcpy(&copy[start], &C[0], 3*sizeof(float));
    }
}

void Models::drawModel(int16_t xAngle, int16_t yAngle, int16_t zAngle, uint8_t color)
{
    modifyAngle(yAngle, Y);
    modifyAngle(xAngle, X);
//    modifyAngle(zAngle, Z);

    param H;
    int16_t current = 1;
    int16_t count = (int16_t)copy[0];
    while(count--)
    {
        int16_t start = current;
        H.value[2] = copy[current++];
        H.value[1] = copy[current++];
        H.value[0] = copy[current++];
//        H.shape[0] = 3;
//        H.shape[1] = 1;
        float I[(int8_t)(s_zAngle.shape[0]*H.shape[1])];
        TensorPort(s_zAngle, H, I);
        memcpy(&copy[start], &I[0], 3*sizeof(float));
    }

    current = 1;
    count = (int16_t)copy[0];
    while(count--)
    {
        param K;
        int16_t start = current;
        K.value[3] = 1.0f;
        K.value[2] = copy[current++];
        K.value[1] = copy[current++];
        K.value[0] = copy[current++];
        K.shape[0] = 4;
//        K.shape[1] = 1;

        float L[(int8_t)(s_Ortho.shape[0]*K.shape[1])];
        TensorPort(s_Ortho, K, L);
        memcpy(&copy[start], &L[0], 3*sizeof(float));
    }

    int8_t offsetX = WIDTH/2;
    int8_t offsetY = HEIGHT/2;

    current = 1;
    count = (int16_t)copy[0];
    while(current < count*3)
    {
        int16_t x1 = copy[current++] + offsetX;
        int16_t y1 = copy[current++] + offsetY;
        current++;
        int16_t x2 = copy[current++] + offsetX;
        int16_t y2 = copy[current++] + offsetY;
        current++;
        int16_t x3 = copy[current++] + offsetX;
        int16_t y3 = copy[current++] + offsetY;
        current++;
        arduboy.fillTriangle(x1, y1, x2, y2, x3, y3, color);
    }
}