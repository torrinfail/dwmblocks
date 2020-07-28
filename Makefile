PREFIX ?= /usr/local
CC ?= cc
output: dwmblocks.c blocks.h
	${CC} dwmblocks.c `pkg-config --cflags x11` `pkg-config --libs x11` -o dwmblocks
clean:
	rm -f *.o *.gch dwmblocks
install: output
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp -f dwmblocks $(DESTDIR)$(PREFIX)/bin
	chmod 755 $(DESTDIR)$(PREFIX)/bin/dwmblocks
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/dwmblocks
