#include "test.h"
#include "emdb.h"
#include "storage/memory.h"
#include <string.h>

uint8_t test_emdb ( ) {
  EMDB *db;
  Entry *entry;
  uint8_t ret;

  db = emdb_create_db(getMemoryStorage(), 1024, NULL);

  check(db != NULL, "database is created");

  entry = emdb_read(db, (unsigned char *) "foo");

  check(entry == NULL, "entry is null after read");

  ret = emdb_write(db, (unsigned char *) "foo", (unsigned char *) "bar", 4);
  check(ret == 1, "write is successful");

  entry = emdb_read(db, (unsigned char *) "foo");

  check(entry->size == 4, "entry size is 4");
  check(strcmp((char *) entry->ptr, "bar") == 0, "entry is correct");

  emdb_free_entry(entry);

  emdb_destroy_db(db);

  done();
}
