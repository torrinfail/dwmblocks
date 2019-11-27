output: dwmblocks.o
	gcc dwmblocks.o -lX11 -o dwmblocks
dwmblocks.o: dwmblocks.c blocks.h
	gcc -c -lX11 dwmblocks.c 
clean:
	rm -f *.o *.gch dwmblocks
install: output
	mkdir -p /usr/local/bin
	cp -f dwmblocks /usr/local/bin
