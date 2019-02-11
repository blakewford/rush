#/bin/bash
python reset.py $1
sleep 2
avrdude -c wiring -v -patmega32u4 -cavr109 -P/dev/ttyACM0 -b57600 -D -Uflash:w:$2:i 

