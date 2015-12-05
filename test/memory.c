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

  ctx = MemoryStorage.create_context(NULL);

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

  ret = MemoryStorage.write(ctx, (unsigned char *) "bar", (unsigned char *) "baz", 4);

  check(ret == 1, "second write succeeded");

  entry = MemoryStorage.read(ctx, (unsigned char *) "bar");

  check(entry->size == 4, "entry size is 4");
  check(strcmp(entry->ptr, "baz") == 0, "entry is correct");

  emdb_free_entry(entry);

  MemoryStorage.delete(ctx, (unsigned char *) "foo");
  entry = MemoryStorage.read(ctx, (unsigned char *) "foo");

  check(entry == NULL, "entry is null after delete called");

  stats = MemoryStorage.stats(ctx);
  check(stats->memory_usage == 116, "memory usage is correctly reported");

  MemoryStorage.destroy_context(ctx);

  done();
}
