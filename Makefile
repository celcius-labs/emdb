# You can put your build options here
-include config.mk

CFLAGS=-I./src -Wall -g
LDFLAGS=-g

SRCS=src/emdb.c

all: test run_tests

doc:
	@doxygen Doxyfile



test/test.o: test/test.c
	$(CC) -c $(CFLAGS) test/test.c -o test/test.o

test/memory.o: test/memory.c
	$(CC) -c $(CFLAGS) test/memory.c -o test/memory.o

test/query.o: test/query.c

obj/emdb.o: src/emdb.c src/emdb.h
	$(CC) -c $(CFLAGS) src/emdb.c -o obj/emdb.o

obj/storage/memory.o: src/storage/memory.c src/storage/memory.h src/emdb.h
	$(CC) -c $(CFLAGS) src/storage/memory.c -o obj/storage/memory.o

obj/query.o: src/query.c src/query.h
	$(CC) -c $(CFLAGS) src/query.c -o obj/query.o

test: test/test.o test/memory.o obj/emdb.o obj/storage/memory.o obj/query.o test/query.o
	$(CC) $(LDFLAGS) test/test.o test/memory.o test/query.o obj/emdb.o obj/storage/memory.o obj/query.o -o test_runner

.PHONY: run_tests clean clean_src

run_tests:
	./test_runner --spec

clean: clean_src clean_test

clean_src:
	@rm -f obj/*.o
	@rm -f obj/storage/*.o

clean_test:
	@rm -f test/*.o
	@rm -f test/test
	@rm -f ./test_runner
