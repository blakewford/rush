#include <Arduboy2.h>

#include "fix.cpp"
#include "model/Models.cpp"

#ifdef PROFILE
#include <chrono>
using namespace std::chrono;
#endif

void rush()
{
	init();
    setup();
    while(gKeepGoing)
    {
        gReportedVerts = 0;
#ifdef PROFILE
    microseconds start = duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch());
#endif
        loop();
#ifdef PROFILE
    microseconds end = duration_cast<microseconds>(high_resolution_clock::now().time_since_epoch());
    if(gDbgAttached)
    {
        static char json[64];
        memset(json, '\0', 64);
        uint16_t length = sprintf(json, "{\"FPS\":%lu,\"Vertices\":\"%u\"}", 1000000/(end.count()-start.count()), gReportedVerts);

        send(gSocket, &length, sizeof(uint16_t), 0);
        send(gSocket, json, length, 0);
    }
#endif
        post();
    }
}

#ifdef __AVR__
int main()
{
    rush();
    return 0;
}
#else
int32_t gCachedArgc = 0;
char* gCachedArgv[64];
static char gArgvStorage[1024];

int32_t main(int32_t argc, char** argv)
{
    gCachedArgc = argc;
    char* storagePointer = gArgvStorage;
    while(argc--)
    {
        gCachedArgv[argc] = storagePointer;
        int32_t length = strlen(argv[argc]);
        strcat(storagePointer, argv[argc]);
        storagePointer+=(length+1);
    }

    rush();

    return 0;
}
#endif
