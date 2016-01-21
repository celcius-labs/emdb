# You can put your build options here
-include config.mk
CC=g++

CFLAGS=-I./src -Wall -g -march=native
LDFLAGS=-g -flto -L. -lemdb

SRCS=src/emdb.cc

OBJS=obj/emdb.o
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

obj/storage/memory.o: src/storage/memory.cc src/storage/memory.h src/emdb.h
	$(CC) -c $(CFLAGS) src/storage/memory.cc -o obj/storage/memory.o

test/memory.o: test/memory.cc src/storage/memory.cc src/storage/memory.h
	$(CC) -c $(CFLAGS) test/memory.cc -o test/memory.o

OBJS += obj/storage/memory.o
TEST += test/memory.o

endif

ifdef EMDB_QUERY
CFLAGS += -DEMDB_QUERY

OBJS += obj/query.o
TEST += test/query.o

obj/query.o: src/query.h
	$(CC) -c $(CFLAGS) src/query.cc -o obj/query.o

test/query.o: test/query.cc
	$(CC) -c $(CFLAGS) test/query.cc -o test/query.o

endif

ifdef EMDB_JSON
CFLAGS += -DEMDB_JSON -I./jsmn

OBJS += obj/json.o jsmn/jsmn.o
TEST += test/json.o

obj/json.o: src/json.h
	$(CC) -c $(CFLAGS) src/json.cc -o obj/json.o

test/json.o: test/json.cc
	$(CC) -c $(CFLAGS) test/json.cc -o test/json.o

jsmn/jsmn.o: jsmn/jsmn.c
	$(CC) -c $(CFLAGS) jsmn/jsmn.cc -o jsmn/jsmn.o

endif


doc:
	@doxygen Doxyfile



test/test.o: test/test.cc
	$(CC) -c $(CFLAGS) test/test.cc -o test/test.o



obj/emdb.o: src/emdb.cc src/emdb.h
	$(CC) -c $(CFLAGS) src/emdb.cc -o obj/emdb.o

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
