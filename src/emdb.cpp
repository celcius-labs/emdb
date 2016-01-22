#include <stdlib.h>
#include <string.h>

#include "emdb.h"
#ifdef DEBUG
#include "debug.h"
#endif

static uint8_t *errors[] = {
  NULL,
  (uint8_t *)"Unable to allocate memory",
  (uint8_t *)"Maximum memory used",
  (uint8_t *)"Query error"
};

EMDB *emdb_create_db (Storage *store, uint32_t max_memory, void *cfg) {
#ifdef DEBUG
  emdb_debug("emdb_create_db(): allocating memory");
#endif
  EMDB *db = (EMDB *) malloc(sizeof(EMDB));

  // error, unable to allocate memory
  if (db == NULL) {
#ifdef DEBUG
    emdb_debug("emdb_create_db(): unable to allocate memory");
#endif
    return NULL;
  }

  db->store = store;
  db->count = 0;
  db->memory = 0;
  db->max_memory = max_memory;
  db->error = 0;

#ifdef DEBUG
emdb_debug("emdb_create_db(): creating store context");
#endif
  db->ctx = store->create_context(cfg);

#ifdef DEBUG
  emdb_debug("emdb_create_db(): returning db");
#endif
  return db;
}

void emdb_destroy_db (EMDB *db) {
  if (db->ctx) {
    db->store->destroy_context(db->ctx);
  }

  free(db);
}

uint8_t *emdb_last_error (EMDB *db) {
  int last_error = db->error;
  if (last_error) {
    db->error = 0;
    return errors[last_error];
  } else {
    // fallback to store error
    return db->store->last_error(db->ctx);
  }
}

uint8_t emdb_write(EMDB *db, uint8_t *key, uint8_t *value, uint16_t size) {
  uint8_t ret;
  Stats *stats;

  // if we are out of memory, set the error and return
  if (db->max_memory && db->memory >= db->max_memory) {
    db->error = 2;

    return 0;
  }

  ret = db->store->store_write(db->ctx, key, value, size);

  // update the statistics
  stats = db->store->stats(db->ctx);

  db->memory = stats->memory_usage;
  db->count = stats->entries;

  return ret;
}

Entry *emdb_read(EMDB *db, uint8_t *key) {
  Entry *ret;

  ret = db->store->store_read(db->ctx, key);

  return ret;
}

uint8_t emdb_delete(EMDB *db, uint8_t *key) {
  uint8_t ret;
  Stats *stats;

  ret = db->store->store_delete(db->ctx, key);

  // update the statistics
  stats = db->store->stats(db->ctx);

  db->memory = stats->memory_usage;
  db->count = stats->entries;

  return ret;
}

void emdb_scan (EMDB *db, void *ctx, void entry_handler(void *, uint8_t *, Entry *), void end_handler(void *), void error_handler(void *, uint8_t *)) {
  db->store->scan(db->ctx, ctx, entry_handler, end_handler, error_handler);
}

Entry *emdb_copy_entry (Entry *entry) {
  Entry *dup = (Entry *) malloc(sizeof (Entry));

  if (dup == NULL) {
    return NULL;
  }

  dup->size = entry->size;
  dup->ptr = (void *) malloc(sizeof (uint8_t) * entry->size);

  if (dup->ptr == NULL) {
    free(dup);
    return NULL;
  }

  memcpy(dup->ptr, entry->ptr, sizeof (uint8_t) * entry->size);

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
