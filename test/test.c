#include <stdio.h>
#include <string.h>

#include "storage/memory.h"
#include "emdb.h"


#include "test.h"


int test_memory ( ) {
  Entry *entry;
  unsigned char ret;
  entry = MemoryStorage.read((unsigned char *) "foo");

  check(entry == NULL, "entry is null after read");

  ret = MemoryStorage.write((unsigned char *) "foo", (unsigned char *) "bar", 4);

  check(ret == 1, "write succeeded");

  entry = MemoryStorage.read((unsigned char *) "foo");

  check(entry->size == 4, "entry size is 4");
  check(strcmp(entry->ptr, "bar") == 0, "entry is correct");

  done();
}

int test_emdb ( ) {
  EMDB *db;

  db = emdb_create_db(&MemoryStorage, 1024);

  check(db != NULL, "database is created");

  done();
}

int main (int argc, char **argv) {
  if (argc > 1) {
    if ((strcmp(argv[1], "--spec") == 0) || (strcmp(argv[1], "-s") == 0)) {
      spec = 1;
    }
  }

  test(test_memory, "test memory storage");
  test(test_emdb, "test emdb");

  printf("\nPASSED: %d\nFAILED: %d\n", test_passed, test_failed);

  return (test_failed > 0);
}
