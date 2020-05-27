PREFIX = /usr/local
DEPS = buffer findandreplace
INCS = `pkg-config --cflags x11` -Ibuffer/include -Ifindandreplace/include
LIBS = `pkg-config --libs x11` findandreplace/obj/replacechr.o
CFLAGS = -std=gnu11 -march=native -Os -DNDEBUG -Wall -Wextra -Wmissing-declarations

.PHONY: clean install uninstall pacman

dwmblocks: dwmblocks.c blocks.h Makefile ${DEPS}
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
