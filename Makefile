# You can put your build options here
-include config.mk
CC=g++

CFLAGS=-I./src -Wall -g -march=native
LDFLAGS=-g -flto -L. -lemdb

SRCS=src/emdb.cpp

OBJS=obj/emdb.o obj/debug.o
TEST=test/test.o


all: build

ifdef LARGE
CFLAGS += -DLARGE
endif

ifdef MEDIUM
CFLAGS += -DMEDIUM
endif

ifdef SMALL
CFLAGS += -DSMALL
endif

ifdef TINY
CFLAGS += -DTINY
endif

ifdef EMDB_MEMORY_STORAGE
CFLAGS += -DEMDB_MEMORY_STORAGE

obj/storage/memory.o: src/storage/memory.cpp src/storage/memory.h src/emdb.h
	$(CC) -c $(CFLAGS) src/storage/memory.cpp -o obj/storage/memory.o

test/memory.o: test/memory.cpp src/storage/memory.cpp src/storage/memory.h
	$(CC) -c $(CFLAGS) test/memory.cpp -o test/memory.o

OBJS += obj/storage/memory.o
TEST += test/memory.o

endif

ifdef EMDB_QUERY
CFLAGS += -DEMDB_QUERY

OBJS += obj/query.o
TEST += test/query.o

obj/query.o: src/query.h
	$(CC) -c $(CFLAGS) src/query.cpp -o obj/query.o

test/query.o: test/query.cpp
	$(CC) -c $(CFLAGS) test/query.cpp -o test/query.o

endif

ifdef EMDB_JSON
CFLAGS += -DEMDB_JSON -I./jsmn

OBJS += obj/json.o jsmn/jsmn.o
TEST += test/json.o

obj/json.o: src/json.h
	$(CC) -c $(CFLAGS) src/json.cpp -o obj/json.o

test/json.o: test/json.cpp
	$(CC) -c $(CFLAGS) test/json.cpp -o test/json.o

jsmn/jsmn.o: jsmn/jsmn.c
	$(CC) -c $(CFLAGS) jsmn/jsmn.c -o jsmn/jsmn.o

endif


doc:
	@doxygen Doxyfile



test/test.o: test/test.cpp
	$(CC) -c $(CFLAGS) test/test.cpp -o test/test.o



obj/emdb.o: src/emdb.cpp src/emdb.h
	$(CC) -c $(CFLAGS) src/emdb.cpp -o obj/emdb.o

obj/debug.o: src/debug.cpp src/debug.h
	$(CC) -c $(CFLAGS) src/debug.cpp -o obj/debug.o

build: $(OBJS) $(TEST)
	$(AR) -cvq libemdb.a $(OBJS)
	$(CC) $(TEST) $(LDFLAGS) -o test_runner

.PHONY: clean clean_src

test: build
	./test_runner --spec

clean: clean_src clean_test clean_lib

clean_src:
	@rm -f obj/*.o
	@rm -f obj/storage/*.o

clean_test:
	@rm -f test/*.o
	@rm -f test/test
	@rm -f ./test_runner

clean_lib:
	@rm -f libemdb.a
