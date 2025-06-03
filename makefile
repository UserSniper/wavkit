CC = mos-cx16-clang
CARGS = -Oz -flto -fnonreentrant -Wno-int-conversion 

WAVKIT = wavkit.o
WAVARGS = -Os -flto -fnonreentrant -Wno-int-conversion 

default: make

make: 
	rm -rf ./OUT
	mkdir ./OUT
	mkdir ./OUT/ASSETS

	cp ./wav/*.WAV ./OUT/ASSETS

	$(CC) $(WAVARGS) -c ./src/lib/wavkit.c -o $(WAVKIT)
	$(CC) $(CARGS) ./src/main.c $(WAVKIT) -o ./OUT/WAVKIT.PRG
	./x16emu/x16emu -debug -zeroram -midline-effects -c816 -mhz 8