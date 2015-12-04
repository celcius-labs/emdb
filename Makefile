CFLAGS=-I./src

all: test


obj/test.o:
	$(CC) -c $(CFLAGS) test/test.c -o obj/test.o

obj/emdb.o:
	$(CC) -c $(CFLAGS) src/emdb.c -o obj/emdb.o

obj/storage/memory.o:
	$(CC) -c $(CFLAGS) src/storage/memory.c -o obj/storage/memory.o

test: obj/test.o obj/emdb.o obj/storage/memory.o
	$(CC) obj/test.o obj/emdb.o obj/storage/memory.o -o test_runner
	./test_runner --spec


clean: clean_src clean_test

clean_src:
	@rm -f obj/*.o
	@rm -f obj/storage/*.o

clean_test:
	@rm -f test/*.o
	@rm -f test/test
	@rm -f ./test_runner
