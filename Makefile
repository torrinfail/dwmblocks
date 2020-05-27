PREFIX = /usr/local
INCS = `pkg-config --cflags x11` -Ibuffer/include
LIBS = `pkg-config --libs x11`
CFLAGS = -march=native -Os -DNDEBUG -Wall -Wextra -Wmissing-declarations

.PHONY: clean install uninstall

dwmblocks: dwmblocks.c blocks.h Makefile buffer
	${CC} -o dwmblocks ${CFLAGS} ${INCS} dwmblocks.c ${LIBS}

blocks.h: blocks.def.h
	cp blocks.def.h blocks.h

clean:
	rm -f *.o *.gch dwmblocks dwmblocks-*-any.pkg.tar.xz

install: dwmblocks
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f dwmblocks ${DESTDIR}${PREFIX}/bin/
	chmod 751 ${DESTDIR}${PREFIX}/bin/dwmblocks

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/dwmblocks

pacman:
	makepkg -cfi
