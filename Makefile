output: dwmblocks.o blocks.o
	gcc dwmblocks.o blocks.o -o dwmblocks
dwmblocks.o: dwmblocks.c
	gcc -c dwmblocks.c 
blocks.o: blocks.h
	gcc -c blocks.h 
clean:
	rm *.o dwmblocks
