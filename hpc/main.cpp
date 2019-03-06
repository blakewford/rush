/*
\file   main.c

\brief  Main source file.

(c) 2018 Microchip Technology Inc. and its subsidiaries.

Subject to your compliance with these terms, you may use Microchip software and any
derivatives exclusively with Microchip products. It is your responsibility to comply with third party
license terms applicable to your use of third party software (including open source software) that
may accompany Microchip software.

THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY
IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS
FOR A PARTICULAR PURPOSE.

IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP
HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO
THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT
OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS
SOFTWARE.
*/

long random(long howsmall, long howbig)
{
    return 0;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Arduboy2.h"
#include "application_manager.h"
#include "cloud/cloud_service.h"

#include "../fix.cpp"

extern Arduboy2Base arduboy;

extern "C"
{

char gJsonBuffer[64];
uint32_t gCycleCount = 0;
uint16_t buildJson(char* buffer, uint16_t bufferSize)
{
    uint16_t length = 0;
    uint32_t fps = 1000/((gCycleCount*1024)/16000);
    if(gReportedVerts > 0)
    {
        memset(gJsonBuffer, '\0', 64);
        length = sprintf(gJsonBuffer, "{\"FPS\":%lu,\"Vertices\":\"%u\"}", fps, gReportedVerts);
    }
    else
    {
        gJsonBuffer[0] = '\0';
        length = 0;
    }

    return length;
}

// This will get called every 1 second only while we have a valid Cloud connection
void sendToCloud(void)
{
    uint16_t length = buildJson(gJsonBuffer, 64);
    if(length > 0)
    {
        CLOUD_publishData((uint8_t *)gJsonBuffer, length);
    }
}

// This handles messages published from the MQTT server when subscribed
void receivedFromCloud(uint8_t* topic, uint8_t* payload)
{
}

int main(void)
{
    application_init();

    setup();

    TCA0.SINGLE.PER = ~0;
    while(gKeepGoing)
    {
        gReportedVerts = 0;
        TCA0.SINGLE.CTRLA |= (TCA_SINGLE_CLKSEL_DIV1024_gc) | (TCA_SINGLE_ENABLE_bm);

        loop();

        TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;
        gCycleCount = TCA0.SINGLE.CNT;
        TCA0.SINGLE.CNT = 0;

        runScheduler();
    }

    return 0;
}
}