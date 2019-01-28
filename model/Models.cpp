#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

#include "Models.h"

extern Arduboy2Base arduboy;

//#define PROFILE

namespace xla
{
    void EigenMatVecF32(float* out, float* lhs, float* rhs, long long m, long long n, long long k, int transpose_lhs, int transpose_rhs)
    {
        assert(0);
    }

    void EigenMatVecF64(double* out, double* lhs, double* rhs, long long m, long long n, long long k, int transpose_lhs, int transpose_rhs)
    {
        assert(0);
    }
}

namespace std
{
    void swap(long long& A, long long& B)
    {
        long long C = A;
        A = B;
        B = C;
    }
}

#include "runtime_single_threaded_matmul.cc"

#define BUFFER_SIZE 16

enum parse_state
{
    CONSTANTS,
    SHAPE,
    NAME,
    UNKNOWN
};

struct param
{
    float value[BUFFER_SIZE];
    float shape[BUFFER_SIZE];
    char name[BUFFER_SIZE];
};

void TensorPort(const param& A, const param& B, float* C)
{
    assert(A.shape[1] == B.shape[0]);
    MatMul(NULL, C, (float*)A.value, (float*)B.value, A.shape[0], B.shape[1], A.shape[1], true, true);
}

enum rotation_axis
{
    X,
    Y,
    Z
};

void rotationEntry(const int16_t angle, param& parameter, rotation_axis axis)
{
    const float radians = (angle%360)*0.0174533;

    parameter.shape[0] = 3;
    parameter.shape[1] = 3;

    switch(axis)
    {
        case X:
            parameter.value[0] = 1;
            parameter.value[1] = 0;
            parameter.value[2] = 0;
            parameter.value[3] = 0;
            parameter.value[4] = cos(radians);
            parameter.value[5] = sin(radians);
            parameter.value[6] = 0;
            parameter.value[7] = -sin(radians);
            parameter.value[8] = cos(radians);
            break;
        case Y:
            parameter.value[0] = cos(radians);
            parameter.value[1] = 0;
            parameter.value[2] = -sin(radians);
            parameter.value[3] = 0;
            parameter.value[4] = 1;
            parameter.value[5] = 0;
            parameter.value[6] = sin(radians);
            parameter.value[7] = 0;
            parameter.value[8] = cos(radians);
            break;
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
        default:
            break;
    }
}

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

const char* ortho = "[1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0], name='plane', shape=[4,4]";

struct vertex
{
    float x;
    float y;
    float z;
};

float copy[4252];
//float copy[1984];

void Models::drawModel(const float* model, int16_t xAngle, int16_t yAngle, int16_t zAngle, uint8_t color)
{
#ifdef PROFILE
        microseconds start = duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch());
#endif
        int16_t count = (int16_t)model[0];
        count*=3;
        count++;

        memcpy(copy, model, count*sizeof(float));

        int16_t current = 1;
        count = (int16_t)copy[0];

        param A, B;
        char buffer[128];
        int32_t valueSize, shapeSize;
        rotationEntry(yAngle, A, Y);
        while(count--)
        {
            int16_t start = current;

            memset(buffer, '\0', 128);
            sprintf(buffer, "[%.8f, %.8f, %.8f], name='vertex', shape=[3,1]\n", copy[current++], copy[current++], copy[current++]);
            parseEntry(buffer, B, valueSize, shapeSize);
            float C[(int32_t)(A.shape[0]*B.shape[1])];
            TensorPort(A, B, C);
            copy[start]     = C[0];
            copy[start + 1] = C[1];
            copy[start + 2] = C[2];
        }

        current = 1;
        count = (int16_t)copy[0];

        param D, E;
        rotationEntry(xAngle, D, X);
        while(count--)
        {
            int16_t start = current;

            sprintf(buffer, "[%.8f, %.8f, %.8f], name='vertex', shape=[3,1]\n", copy[current++], copy[current++], copy[current++]);
            parseEntry(buffer, E, valueSize, shapeSize);
            float F[(int32_t)(D.shape[0]*E.shape[1])];
            TensorPort(D, E, F);
            copy[start]     = F[0];
            copy[start + 1] = F[1];
            copy[start + 2] = F[2];
        }

        current = 1;
        count = (int16_t)copy[0];

        param G, H;
        rotationEntry(zAngle, G, Z);
        while(count--)
        {
            int16_t start = current;

            sprintf(buffer, "[%.8f, %.8f, %.8f], name='vertex', shape=[3,1]\n", copy[current++], copy[current++], copy[current++]);
            parseEntry(buffer, H, valueSize, shapeSize);
            float I[(int32_t)(G.shape[0]*H.shape[1])];
            TensorPort(G, H, I);
            copy[start]     = I[0];
            copy[start + 1] = I[1];
            copy[start + 2] = I[2];
        }

        current = 1;
        count = (int16_t)copy[0];
        while(count--)
        {
            int16_t start = current;

            param J, K;
            parseEntry(ortho, J, valueSize, shapeSize);
            sprintf(buffer, "[%.8f, %.8f, %.8f, 1.0], name='vertex', shape=[4,1]\n", copy[current++], copy[current++], copy[current++]);
            parseEntry(buffer, K, valueSize, shapeSize);
            float L[(int32_t)(J.shape[0]*K.shape[1])];
            TensorPort(J, K, L);
            copy[start]     = L[0];
            copy[start + 1] = L[1];
            copy[start + 2] = L[2];
        }

        int8_t offsetX = WIDTH/2;
        int8_t offsetY = HEIGHT/2;

        current = 1;
        count = (int16_t)copy[0];
        while(count-=3)
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

#ifdef PROFILE
        microseconds end = duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch());
        printf("Microseconds %lld\n", end.count()-start.count());
#endif
}
