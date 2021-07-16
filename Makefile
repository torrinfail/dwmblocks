PREFIX  ?= /usr/local
CC      ?= cc
CFLAGS  = -pedantic -Wall -Wno-deprecated-declarations -Os
LDFLAGS = -lX11
# FreeBSD
#LDFLAGS += -L/usr/local/lib -I/usr/local/include

all: options dwmblocks

options:
	@echo dwmblocks build options:
	@echo "CFLAGS  = ${CFLAGS}"
	@echo "LDFLAGS = ${LDFLAGS}"
	@echo "CC      = ${CC}"

dwmblocks: dwmblocks.c blocks.def.h blocks.h
	${CC} -o dwmblocks dwmblocks.c ${CFLAGS} ${LDFLAGS}

blocks.h:
	cp blocks.def.h $@

clean:
	rm -rf *.o *.gch dwmblocks

install: dwmblocks
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f dwmblocks ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/dwmblocks

uninstall:
	rm -rf ${DESTDIR}${PREFIX}/bin/dwmblocks

.PHONY: all options clean install uninstall
