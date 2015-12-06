#include <stdio.h>

#include "storage/memory.h"
#include "emdb.h"


int main ( ) {
  EMDB *db;
  Entry *entry;

  db = emdb_create_db(&MemoryStorage, 1024, NULL);

  emdb_write(db, (unsigned char *)"key", (unsigned char *)"value", 6);
  entry = emdb_read(db, (unsigned char *)"key");

  printf("found entry: %s\n", (char *)entry->ptr);

  emdb_free_entry(entry);

  emdb_destroy_db(db);

  return 1;
}
