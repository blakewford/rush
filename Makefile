CC=g++
SRC=main.cpp
EXTRA=engine/engine.cpp -lSDL2 -DPROGMEM= -O3
GAME=rush
TARGET=atmega32u4

game: $(SRC)
	$(CC) -I/usr/include/SDL2 -gdwarf-4 -std=c++11 $(SRC) $(EXTRA) -o $(GAME) -I. -I./engine -I./model -I/home/blakewford/tensorflow -I./model/eigen -include port.h -Wno-narrowing -fpermissive -w -DNDEBUG -fdata-sections -ffunction-sections -Wl,--gc-sections

avr:
	make GAME=avr-rush CC=/opt/microchip/avr8-gnu-toolchain-linux_x86_64/bin/avr-g++ EXTRA="-I./model/port -include stdlib.h -include sys/types.h -include port.h -include avr/io.h -include string.h -mmcu=$(TARGET) ./model/port.c ./engine/Arduboy2.cpp -fno-threadsafe-statics -Wl,-u,vfprintf -lprintf_flt -Os"
	avr-objcopy -Ielf32-avr -Oihex avr-rush rush.hex

clean:
	-@rm $(GAME)
	-@rm avr-rush
	-@rm rush.hex
