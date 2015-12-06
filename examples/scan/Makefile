CFLAGS=-I../../src -Wall -g
LDFLAGS=-g

all: scan

scan.o: scan.c
	$(CC) -c $(CFLAGS) scan.c -o scan.o

scan: scan.o
	$(CC) $(LDFLAGS) scan.o ../../obj/emdb.o ../../obj/storage/memory.o -o scan

clean:
	@rm -f *.o scan
