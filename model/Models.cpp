#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#include "Models.h"

#include <Arduboy2.h>

extern Arduboy2Base arduboy;
extern uint16_t gReportedVerts;

#include "runtime_single_threaded_matmul.cc"

enum parse_state: int8_t
{
    CONSTANTS,
    SHAPE,
    NAME,
    UNKNOWN
};

void MatMul3x1(float* C, const float* A, const float* B)
{
    C[0] = (A[0]*B[0]) + (A[1]*B[1]) + (A[2]*B[2]);
    C[1] = (A[3]*B[0]) + (A[4]*B[1]) + (A[5]*B[2]);
    C[2] = (A[6]*B[0]) + (A[7]*B[1]) + (A[8]*B[2]);
}

void MatMul3x1Sparse(float* C, const float* A, const float* B)
{
    C[0] = (A[0]*B[0]);
    C[1] = (A[4]*B[1]);
    C[2] = (A[8]*B[2]);
}

void MatMul4x1Sparse(float* C, const float* A, const float* B)
{
    C[0] = (A[0]*B[0]);
    C[1] = (A[5]*B[1]);
    C[2] = 0;
    C[3] = (A[15]*B[3]);
}

param X_AT_15_DEGREES;

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

float copy[19*sizeof(float)];

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
//    s_zAngle.shape[0] = 3;
    s_zAngle.shape[1] = 3;

    s_Ortho.shape[0] = 4;
    s_Ortho.shape[1] = 4;
    s_Ortho.value[0] = ortho[0][0];
//    s_Ortho.value[1] = ortho[0][1];
//    s_Ortho.value[2] = ortho[0][2];
//    s_Ortho.value[3] = ortho[0][3];
//    s_Ortho.value[4] = ortho[1][0];
    s_Ortho.value[5] = ortho[1][1];
//    s_Ortho.value[6] = ortho[1][2];
//    s_Ortho.value[7] = ortho[1][3];
//    s_Ortho.value[8] = ortho[2][0];
//    s_Ortho.value[9] = ortho[2][1];
//    s_Ortho.value[10] = ortho[2][2];
//    s_Ortho.value[11] = ortho[2][3];
//    s_Ortho.value[12] = ortho[3][0];
//    s_Ortho.value[13] = ortho[3][1];
//    s_Ortho.value[14] = ortho[3][2];
    s_Ortho.value[15] = ortho[3][3];

    X_AT_15_DEGREES.value[0] = 1;
//    X_AT_15_DEGREES.value[1] = 0;
//    X_AT_15_DEGREES.value[2] = 0;
//    X_AT_15_DEGREES.value[3] = 0;
    X_AT_15_DEGREES.value[4] = 0.965925813;
    X_AT_15_DEGREES.value[5] = 0.258819163;
    X_AT_15_DEGREES.value[6] = -0.258819163;
    X_AT_15_DEGREES.value[7] = 0.965925813;
//    X_AT_15_DEGREES.value[8] = 0;
//    X_AT_15_DEGREES.shape[0] = 3;
    X_AT_15_DEGREES.shape[1] = 3;
}

void Models::drawModel(const float* model, int16_t xAngle, int16_t yAngle, int16_t zAngle, uint8_t color)
{
    int16_t count = (int16_t)model[0];
    count*=3;
    gReportedVerts += count;

    count++;
    memcpy(copy, model, count*sizeof(float));
    drawModel(xAngle, yAngle, zAngle, color);
}

void Models::drawCompressedModel(const uint8_t* model, const float* map, int16_t xAngle, int16_t yAngle, int16_t zAngle, uint8_t color)
{
    int16_t count = (int16_t)map[0];
    count*=3;
    gReportedVerts += count;

    copy[0] = 6;
    int16_t ndx = 0;
    while(ndx < count)
    {
        copy[1] = map[pgm_read_byte(&model[ndx])];
        copy[2] = map[pgm_read_byte(&model[ndx+1])];
        copy[3] = map[pgm_read_byte(&model[ndx+2])];
        copy[4] = map[pgm_read_byte(&model[ndx+3])];
        copy[5] = map[pgm_read_byte(&model[ndx+4])];
        copy[6] = map[pgm_read_byte(&model[ndx+5])];
        copy[7] = map[pgm_read_byte(&model[ndx+6])];
        copy[8] = map[pgm_read_byte(&model[ndx+7])];
        copy[9] = map[pgm_read_byte(&model[ndx+8])];
        copy[10] = map[pgm_read_byte(&model[ndx+9])];
        copy[11] = map[pgm_read_byte(&model[ndx+10])];
        copy[12] = map[pgm_read_byte(&model[ndx+11])];
        copy[13] = map[pgm_read_byte(&model[ndx+12])];
        copy[14] = map[pgm_read_byte(&model[ndx+13])];
        copy[15] = map[pgm_read_byte(&model[ndx+14])];
        copy[16] = map[pgm_read_byte(&model[ndx+15])];
        copy[17] = map[pgm_read_byte(&model[ndx+16])];
        copy[18] = map[pgm_read_byte(&model[ndx+17])];
        drawModel(xAngle, yAngle, zAngle, color);
        ndx+=18;
    }
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
        float C[3];
        MatMul3x1(C, A.value, B.value);
        copy[start]   = C[0];
        copy[start+1] = C[1];
        copy[start+2] = C[2];
    }
}

void Models::modifyXAngle()
{
    param B;
    int16_t current = 1;
    int16_t count = (int16_t)copy[0];
    while(count--)
    {
        int16_t start = current;
        B.value[2] = copy[current++];
        B.value[1] = copy[current++];
        B.value[0] = copy[current++];
//        B.shape[0] = 3;
//        B.shape[1] = 1;
        float C[3];
        MatMul3x1(C, X_AT_15_DEGREES.value, B.value);
        copy[start]   = C[0];
        copy[start+1] = C[1];
        copy[start+2] = C[2];
    }
}

void Models::drawModel(int16_t xAngle, int16_t yAngle, int16_t zAngle, uint8_t color)
{

    modifyAngle(yAngle, Y);
#ifdef PHASE2
    modifyXAngle();
#else
    modifyAngle(xAngle, X);
#endif
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
        float I[3];
        MatMul3x1Sparse(I, s_zAngle.value, H.value);
        copy[start]   = I[0];
        copy[start+1] = I[1];
        copy[start+2] = I[2];
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

        float L[4];
        MatMul4x1Sparse(L, s_Ortho.value, K.value);
        copy[start]   = L[0];
        copy[start+1] = L[1];
        copy[start+2] = L[2];
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
//        arduboy.fillTriangle(x1, y1, x2, y2, x3, y3, color);
        fillTriangle(x1, y1, x2, y2, x3, y3, color);
    }
}