%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@


clean: clean_src clean_test
clean_src:
	@rm -f src/*.o
	@rm -f src/storage/*.o
clean_test:
	@rm -f test/*.o
	@rm -f test/test
