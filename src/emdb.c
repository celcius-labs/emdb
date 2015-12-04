#include <stdlib.h>
#include <string.h>

#include "emdb.h"

EMDB *emdb_create_db (Storage *store, unsigned int max_memory) {
  EMDB *db = (EMDB *) malloc(sizeof(EMDB));

  // error, unable to allocate memory
  if (db == NULL) {
    return NULL;
  }

  db->store = store;
  db->count = 0;
  db->memory = 0;
  db->max_memory = max_memory;

  return db;
}

Entry *emdb_copy_entry (Entry *entry) {
  Entry *dup = (Entry *) malloc(sizeof (Entry));

  if (dup == NULL) {
    return NULL;
  }

  dup->size = entry->size;
  dup->ptr = (void *) malloc(sizeof (unsigned char) * entry->size);

  if (dup->ptr == NULL) {
    free(dup);
    return NULL;
  }

  memcpy(dup->ptr, entry->ptr, sizeof (unsigned char) * entry->size);

  return dup;
}
