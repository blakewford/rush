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

// This handles messages published from the MQTT server when subscribed
void receivedFromCloud(uint8_t* topic, uint8_t* payload)
{
}

uint32_t gCycleCount = 0;

// This will get called every 1 second only while we have a valid Cloud connection
void sendToCloud(void)
{
    static char json[70];

    int len = sprintf(json, "{\"FPS\":%lu,\"Vertices\":\"%u\"}", 1000/((gCycleCount*1024)/16000), gReportedVerts);
    if(len > 0)
    {
        CLOUD_publishData((uint8_t *)json, len);
    }
}

int main(void)
{
    application_init();

    setup();

    TCA0.SINGLE.PER = ~0;
    while(gKeepGoing)
    {
        TCA0.SINGLE.CTRLA |= (TCA_SINGLE_CLKSEL_DIV1024_gc) | (TCA_SINGLE_ENABLE_bm);

        gReportedVerts = 0;
        loop();

        TCA0.SINGLE.CTRLA &= ~TCA_SINGLE_ENABLE_bm;
        gCycleCount = TCA0.SINGLE.CNT;
        TCA0.SINGLE.CNT = 0;

        runScheduler();
    }

    return 0;
}
}