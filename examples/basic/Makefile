CFLAGS=-I../../src -Wall -g
LDFLAGS=-g

all: basic

basic.o: basic.c
	$(CC) -c $(CFLAGS) basic.c -o basic.o

basic: basic.o
	$(CC) $(LDFLAGS) basic.o ../../obj/emdb.o ../../obj/storage/memory.o -o basic

clean:
	@rm -f *.o basic
