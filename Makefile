INCS=`pkg-config --cflags x11` -Ibuffer/include
LIBS=`pkg-config --libs x11`

output: dwmblocks.c blocks.h
	cc -o dwmblocks dwmblocks.c ${INCS} ${LIBS}
blocks.h: blocks.def.h
	cp blocks.def.h blocks.h
clean:
	rm -f *.o *.gch dwmblocks
install: output
	mkdir -p /usr/local/bin
	cp -f dwmblocks /usr/local/bin
