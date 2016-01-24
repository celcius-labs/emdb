#include <stdio.h>
#include <string.h>

#include "storage/memory.h"
#include "emdb.h"


#include "test.h"


uint8_t test_memory ( ) {
  Entry *entry;
  unsigned char ret;
  void *ctx;
  Storage *store = getMemoryStorage();

  ctx = store->create_context(NULL);

  entry = store->store_read(ctx, (unsigned char *) "foo");

  check(entry == NULL, "entry is null after read");

  ret = store->store_write(ctx, (unsigned char *) "foo", (unsigned char *) "bar", 4);

  check(ret == 1, "write succeeded");

  entry = store->store_read(ctx, (unsigned char *) "foo");

  check(entry->size == 4, "entry size is 4");
  check(strcmp((char *) entry->ptr, "bar") == 0, "entry is correct");

  emdb_free_entry(entry);

  ret = store->store_write(ctx, (unsigned char *) "bar", (unsigned char *) "baz", 4);

  check(ret == 1, "second write succeeded");

  entry = store->store_read(ctx, (unsigned char *) "bar");

  check(entry->size == 4, "entry size is 4");
  check(strcmp((char *) entry->ptr, "baz") == 0, "entry is correct");

  emdb_free_entry(entry);

  store->store_delete(ctx, (unsigned char *) "foo");
  entry = store->store_read(ctx, (unsigned char *) "foo");

  check(entry == NULL, "entry is null after delete called");

  store->destroy_context(ctx);

  done();
}

uint8_t test_context_isolation ( ) {
  Entry *entry;
  unsigned char ret;
  void *ctx1;
  void *ctx2;
  Storage *store = getMemoryStorage();

  ctx1 = store->create_context(NULL);
  ctx2 = store->create_context(NULL);


  ret = store->store_write(ctx1, (unsigned char *) "foo", (unsigned char *) "bar", 4);

  check(ret == 1, "write to context 1 succeeded");

  entry = store->store_read(ctx1, (unsigned char *) "foo");

  check(entry->size == 4, "entry size is 4");
  check(strcmp((char *) entry->ptr, "bar") == 0, "entry is correct");

  emdb_free_entry(entry);

  entry = store->store_read(ctx2, (unsigned char *) "foo");

  check(entry == NULL, "entry is null after read on context 2");

  ret = store->store_write(ctx1, (unsigned char *) "bar", (unsigned char *) "baz", 4);

  check(ret == 1, "second write to context 1 succeeded");

  entry = store->store_read(ctx1, (unsigned char *) "bar");

  check(entry->size == 4, "entry size is 4");
  check(strcmp((char *) entry->ptr, "baz") == 0, "entry is correct");

  emdb_free_entry(entry);

  entry = store->store_read(ctx2, (unsigned char *) "bar");

  check(entry == NULL, "entry is null after read on context 2");

  store->store_delete(ctx1, (unsigned char *) "foo");
  entry = store->store_read(ctx1, (unsigned char *) "foo");

  check(entry == NULL, "entry is null after delete called");

  store->destroy_context(ctx1);
  store->destroy_context(ctx2);

  done();
}
