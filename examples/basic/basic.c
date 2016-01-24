#include <stdio.h>

#include "storage/memory.h"
#include "emdb.h"


int main ( ) {
  EMDB *db;
  Entry *entry;
  Storage *store = getMemoryStorage();


  db = emdb_create_db(store, NULL);

  emdb_write(db, (uint8_t *)"key", (uint8_t *)"value", 6);
  entry = emdb_read(db, (unsigned char *)"key");

  printf("found entry: %s\n", (char *)entry->ptr);

  emdb_free_entry(entry);

  emdb_destroy_db(db);

  return 1;
}
