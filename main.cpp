#include <Arduboy2.h>

extern Arduboy2Base arduboy;

#include "fix.cpp"
#include "model/Models.cpp"

void rush()
{
	init();
    setup();
    while(gKeepGoing)
    {
        loop();
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
