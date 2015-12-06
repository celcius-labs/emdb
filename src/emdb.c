#include <stdlib.h>
#include <string.h>

#include "emdb.h"

static char *errors[] = {
  NULL,
  "Unable to allocate memory",
  "Maximum memory used"
};

EMDB *emdb_create_db (Storage *store, unsigned int max_memory, void *cfg) {
  EMDB *db = (EMDB *) malloc(sizeof(EMDB));

  // error, unable to allocate memory
  if (db == NULL) {
    return NULL;
  }

  db->store = store;
  db->count = 0;
  db->memory = 0;
  db->max_memory = max_memory;
  db->error = 0;
  db->ctx = store->create_context(cfg);

  return db;
}

void emdb_destroy_db (EMDB *db) {
  if (db->ctx) {
    db->store->destroy_context(db->ctx);
  }

  free(db);
}

char *emdb_last_error (EMDB *db) {
  int last_error = db->error;
  if (last_error) {
    db->error = 0;
    return errors[last_error];
  } else {
    // fallback to store error
    return db->store->last_error(db->ctx);
  }
}

unsigned char emdb_write(EMDB *db, unsigned char *key, unsigned char *value, int size) {
  unsigned char ret;
  Stats *stats;

  // if we are out of memory, set the error and return
  if (db->max_memory && db->memory >= db->max_memory) {
    db->error = 2;

    return 0;
  }

  ret = db->store->write(db->ctx, key, value, size);

  // update the statistics
  stats = db->store->stats(db->ctx);

  db->memory = stats->memory_usage;
  db->count = stats->entries;

  return ret;
}

Entry *emdb_read(EMDB *db, unsigned char *key) {
  Entry *ret;

  ret = db->store->read(db->ctx, key);

  return ret;
}

unsigned char emdb_delete(EMDB *db, unsigned char *key) {
  unsigned char ret;
  Stats *stats;

  ret = db->store->delete(db->ctx, key);

  // update the statistics
  stats = db->store->stats(db->ctx);

  db->memory = stats->memory_usage;
  db->count = stats->entries;

  return ret;
}

void emdb_scan (EMDB *db, void entry_handler(unsigned char *, Entry *), void end_handler(), void error_handler(char *)) {
  db->store->scan(db->ctx, entry_handler, end_handler, error_handler);
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

void emdb_free_entry (Entry *entry) {
  if (entry == NULL) {
    return;
  }

  if (entry->ptr) {
    free(entry->ptr);
  }

  free(entry);
}
