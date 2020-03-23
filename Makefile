output: dwmblocks.c blocks.h
	cc `pkg-config --cflags x11` `pkg-config --libs x11` -Wall -pedantic dwmblocks.c -o dwmblocks
clean:
	rm -f *.o *.gch dwmblocks
install: output
	mkdir -p ~/.local/bin/desktop/
	cp -f dwmblocks ~/.local/bin/desktop/
