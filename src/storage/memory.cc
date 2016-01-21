#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "memory.h"

static MemoryKey *find_key (void *, uint8_t *);
static Entry *store_read (void *, uint8_t *);
static uint8_t store_write (void *, uint8_t *, uint8_t *, uint16_t);
static uint8_t store_delete (void *, uint8_t *);
static Stats *stats (void *);
static uint8_t *last_error (void *);
static void scan (void *, void *, void (*)(void *, uint8_t *, Entry *), void (*)(void *), void (*)(void *, uint8_t *));
static void *create_context (void *);
static void destroy_context (void *);

// custom malloc/free for statistics
static void *_malloc(void *, uint16_t);
static void _free(void *, void *);

// initialize the storage engine
Storage MemoryStorage = {
  store_write,
  store_read,
  store_delete,
  scan,
  stats,
  last_error,
  create_context,
  destroy_context,
  0
};

// list of known errors
static uint8_t *errors[] = {
  NULL,
  (uint8_t *) "Unable to allocate memory",
  (uint8_t *) "Unknown state"
};

// context to run within
typedef struct MemoryCtx {
  uint8_t error;
  MemoryKey *head;
  Stats *stats;
} MemoryCtx;

Storage *getMemoryStorage ( ) {
  return &MemoryStorage;
}

static Entry *store_read (void *ctx, uint8_t *key) {
  MemoryKey *current = find_key(ctx, key);

  if (current == NULL) {
    return NULL;
  } else {
    return emdb_copy_entry(current->entry);
  }
}

static uint8_t store_write (void *ctx, uint8_t *key, uint8_t *value, uint16_t size) {
  MemoryKey *current = find_key(ctx, key);
  MemoryCtx *context = (MemoryCtx *) ctx;

  uint16_t key_size = strlen((const char *) key) + 1;

  if (current == NULL) {
    // new entry, create it, and add it to the list as the head
    current = (MemoryKey *) _malloc(ctx, sizeof (MemoryKey));

    // failed to allocate memory
    if (current == NULL) {
      context->error = 1;
      return 0;
    }

    // allocate the memory for the key
    current->key = (Key *) _malloc(ctx, sizeof (Key));

    // failed to allocate memory
    if (current->key == NULL) {
      _free(ctx, current);
      context->error = 1;
      return 0;
    }

    current->key->key = (uint8_t *) _malloc(ctx, sizeof (uint8_t) * key_size);

    // falled to allocate memory
    if (current->key->key == NULL) {
      _free(ctx, current->key);
      _free(ctx, current);
      context->error = 1;
      return 0;
    }

    current->key->size = key_size;
    memcpy(current->key->key, key, sizeof (uint8_t) * key_size);

    // allocate memory for the Entry
    current->entry = (Entry *) _malloc(ctx, sizeof (Entry));

    // failed to allocate memory
    if (current->entry == NULL) {
      _free(ctx, current->key->key);
      _free(ctx, current->key);
      _free(ctx, current);
      context->error = 1;
      return 0;
    }

    // allocate the memory for the data itself
    current->entry->ptr = (void *) _malloc(ctx, sizeof (uint8_t) * size);

    // unable to malloc, free up the container and return 0
    if (current->entry->ptr == NULL) {
      _free(ctx, current->key->key);
      _free(ctx, current->key);
      _free(ctx, current->entry);
      _free(ctx, current);
      context->error = 1;
      return 0;
    }

    memcpy(current->entry->ptr, value, sizeof(uint8_t) * size);
    current->entry->size = size;
    current->next = context->head;
    context->head = current;
    context->stats->entries++;
  } else {
    // replace the existing data with the new data
    if (current->entry->ptr) {
      _free(ctx, current->entry->ptr);
    }

    current->entry->ptr = (void *) _malloc(ctx, sizeof (uint8_t) * size);

    if (current->entry->ptr == NULL) {
      context->error = 1;
      return 0;
    }

    memcpy(current->entry->ptr, value, sizeof (uint8_t) * size);
    current->entry->size = size;
  }

  return 1;
}

static uint8_t store_delete (void *ctx, uint8_t *key) {
  MemoryCtx *context = (MemoryCtx *) ctx;
  MemoryKey *current = find_key(ctx, key);
  MemoryKey *prev;

  if (current == NULL) {
    return 0;
  }

  // find the entry and remove it,
  // if the head, point head to next
  if (current == context->head) {
    context->head = current->next;
  } else {
    // find the parent entry
    prev = context->head;
    while (prev && prev->next != current) {
      prev = prev->next;
    }

    if (prev == NULL) {
      // odd, not found
      context->error = 2;
      return 0;
    }

    prev->next = current->next;
  }

  context->stats->entries--;

  _free(ctx, current->key->key);
  _free(ctx, current->key);
  _free(ctx, current->entry->ptr);
  _free(ctx, current->entry);
  _free(ctx, current);

  return 1;
}

static MemoryKey *find_key (void *ctx, uint8_t *key) {
  MemoryCtx *context = (MemoryCtx *) ctx;
  MemoryKey *current;
  uint16_t len = strlen((const char *) key) + 1;

  current = context->head;

  while (current != NULL) {
    if (current->key->size == len) {
      if (memcmp(current->key->key, key, len * sizeof(uint8_t)) == 0) {
        return current;
      }
    }

    current = current->next;
  }

  return NULL;
}

Stats *stats (void *ctx) {
  MemoryCtx *context = (MemoryCtx *) ctx;

  return context->stats;
}

static void scan (void *ctx, void *user_ctx, void (*dataHandler)(void *, uint8_t *, Entry *), void (*endHandler)(void *), void (*errorHandler)(void *, uint8_t *)) {
  MemoryCtx *context = (MemoryCtx *) ctx;
  MemoryKey *current;

  current = context->head;
  while (current != NULL) {
    dataHandler(user_ctx, current->key->key, emdb_copy_entry(current->entry));

    current = current->next;
  }

  endHandler(user_ctx);
}

static uint8_t *last_error (void *ctx) {
  MemoryCtx *context = (MemoryCtx *) ctx;
  uint8_t cur = context->error;
  if (cur) {
    context->error = 0;
    return errors[cur];
  } else {
    return NULL;
  }
}

static void *create_context (void *cfg) {
  MemoryCtx *ctx = (MemoryCtx *) malloc(sizeof(MemoryCtx));

  ctx->error = 0;
  ctx->head = NULL;
  ctx->stats = (Stats *) malloc(sizeof(Stats));
  ctx->stats->memory_usage = sizeof(Stats) + sizeof(MemoryCtx);
  ctx->stats->entries = 0;

  return (void *) ctx;
}

static void destroy_context (void *ctx) {
  MemoryCtx *context = (MemoryCtx *) ctx;
  MemoryKey *current = context->head;
  MemoryKey *last;

  while (current != NULL) {
    last = current;
    current = last->next;

    if (last->key) {
      if (last->key->key) {
        _free(ctx, last->key->key);
      }

      _free(ctx, last->key);
    }

    if (last->entry) {
      if (last->entry->ptr) {
        _free(ctx, last->entry->ptr);
      }

      _free(ctx, last->entry);
    }

    _free(ctx, last);
  }

  free(context->stats);
  free(ctx);
}


static void *_malloc (void *ctx, uint16_t size) {
  MemoryCtx *context = (MemoryCtx *) ctx;
  void *ptr;
  void *new_ptr;
  uint16_t *sz;

  size += sizeof(uint16_t);

  ptr = malloc(size);
  context->stats->memory_usage += size;

  sz = (uint16_t *) ptr;
  *sz = size;

  new_ptr = (void *)((uint16_t *) ptr + sizeof(uint16_t));

  return new_ptr;
}

static void _free (void *ctx, void *ptr) {
  MemoryCtx *context = (MemoryCtx *) ctx;
  uint16_t *sz;
  void *true_ptr;

  true_ptr = (void *) ((uint16_t *) ptr - sizeof(uint16_t));

  sz = (uint16_t *) true_ptr;

  context->stats->memory_usage -= *sz;

  free(true_ptr);
}
