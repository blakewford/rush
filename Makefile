CC=g++
SRC=model/Models.cpp main.cpp
EXTRA=engine/engine.cpp -lSDL2 -DPROGMEM= -O3

#make CC=/opt/microchip/avr8-gnu-toolchain-linux_x86_64/bin/avr-g++ EXTRA="-I./model/port -include stdlib.h -include sys/types.h -include port.h -include avr/io.h -include string.h -mmcu=atmega32u4 ./model/port.c ./engine/Arduboy2.cpp -fno-threadsafe-statics -Wl,-u,vfprintf -lprintf_flt -Os"
rush: $(SRC)
	$(CC) -I/usr/include/SDL2 -gdwarf-4 -std=c++11 $(SRC) $(EXTRA) -o $@ -I. -I./engine -I./model -I/home/blakewford/tensorflow -I./model/eigen -include port.h -Wno-narrowing -fpermissive -w -DNDEBUG -fdata-sections -ffunction-sections -Wl,--gc-sections

clean:
	rm rush
