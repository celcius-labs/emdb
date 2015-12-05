#include <stdio.h>
#include <string.h>

#include "storage/memory.h"
#include "emdb.h"


#include "test.h"


int test_memory ( ) {
  Entry *entry;
  unsigned char ret;
  Stats *stats;
  void *ctx;

  ctx = MemoryStorage.create_context();

  entry = MemoryStorage.read(ctx, (unsigned char *) "foo");

  check(entry == NULL, "entry is null after read");

  ret = MemoryStorage.write(ctx, (unsigned char *) "foo", (unsigned char *) "bar", 4);

  check(ret == 1, "write succeeded");

  entry = MemoryStorage.read(ctx, (unsigned char *) "foo");

  check(entry->size == 4, "entry size is 4");
  check(strcmp(entry->ptr, "bar") == 0, "entry is correct");

  emdb_free_entry(entry);

  stats = MemoryStorage.stats(ctx);
  check(stats->memory_usage == 116, "memory usage is correctly reported");

  MemoryStorage.delete(ctx, (unsigned char *) "foo");
  entry = MemoryStorage.read(ctx, (unsigned char *) "foo");

  check(entry == NULL, "entry is null after delete called");

  stats = MemoryStorage.stats(ctx);
  check(stats->memory_usage == 32, "memory usage is correctly reported");

  MemoryStorage.destroy_context(ctx);

  done();
}

int test_emdb ( ) {
  EMDB *db;

  db = emdb_create_db(&MemoryStorage, 1024, NULL);

  check(db != NULL, "database is created");

  emdb_destroy_db(db);

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
