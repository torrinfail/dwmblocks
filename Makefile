output: dwmblocks.c blocks.h
	cc `pkg-config --cflags x11` `pkg-config --libs x11` dwmblocks.c -o dwmblocks
clean:
	rm -f *.o *.gch dwmblocks
install: output
	mkdir -p /usr/local/bin
	cp -f dwmblocks /usr/local/bin
