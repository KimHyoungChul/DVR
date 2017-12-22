all: stuntd stunt

SRC = util.c stuntd.c stunt.c util.h stunt.h packets.c packets.h client.h print.c print.h tests.c tests.h sockc.h sockc.c

CFLAGS = -DLINUX -Wall -g
LFLAGS = -lpcap -lpthread -ldl
CC = gcc ${CFLAGS}

include .deps

.deps: ${SRC}
	${CC} -MM *.c > .deps

stunt: stunt.o util.o packets.o print.o tests.o sockc.o
	${CC} ${LFLAGS} -o $@ $^

stuntd: stuntd.o util.o /usr/lib/libipq.a /usr/lib/libiptc.a
	${CC} ${LFLAGS} -rdynamic -o $@  $^

clean:
	rm -f *.o stunt stuntd
