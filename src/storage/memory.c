#include <string.h>
#include <stdlib.h>

#include "memory.h"

static MemoryKey *find_key (void *, unsigned char *);
static Entry *read (void *, unsigned char *);
static unsigned char write (void *, unsigned char *, unsigned char *, int);
static unsigned char delete (void *, unsigned char *);
static Stats *stats (void *);
static char *last_error (void *);
static void scan (void *, void (*)(unsigned char *, unsigned char *), void (*)(), void (*)(char *));
static void *create_context ( );
static void destroy_context (void *);

// custom malloc/free for statistics
static void *_malloc(void *, unsigned int);
static void _free(void *, void *);

// initialize the storage engine
Storage MemoryStorage = {
  write,
  read,
  delete,
  scan,
  stats,
  last_error,
  create_context,
  destroy_context
};

// list of known errors
static char *errors[] = {
  NULL,
  "Unable to allocate memory",
  "Unknown state"
};

// context to run within
typedef struct MemoryCtx {
  int error;
  MemoryKey *head;
  Stats *stats;
} MemoryCtx;


static Entry *read (void *ctx, unsigned char *key) {
  MemoryKey *current = find_key(ctx, key);

  if (current == NULL) {
    return NULL;
  } else {
    return emdb_copy_entry(current->entry);
  }
}

static unsigned char write (void *ctx, unsigned char *key, unsigned char *value, int size) {
  MemoryKey *current = find_key(ctx, key);
  MemoryCtx *context = (MemoryCtx *) ctx;

  int key_size = strlen((const char *) key) + 1;

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

    current->key->key = (unsigned char *) _malloc(ctx, sizeof (unsigned char) * key_size);

    // falled to allocate memory
    if (current->key->key == NULL) {
      _free(ctx, current->key);
      _free(ctx, current);
      context->error = 1;
      return 0;
    }

    current->key->size = key_size;
    memcpy(current->key->key, key, sizeof (unsigned char) * key_size);

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
    current->entry->ptr = (void *) _malloc(ctx, sizeof (unsigned char) * size);

    // unable to malloc, free up the container and return 0
    if (current->entry->ptr == NULL) {
      _free(ctx, current->key->key);
      _free(ctx, current->key);
      _free(ctx, current->entry);
      _free(ctx, current);
      context->error = 1;
      return 0;
    }

    memcpy(current->entry->ptr, value, sizeof(unsigned char) * size);
    current->entry->size = size;
    current->next = context->head;
    context->head = current;
  } else {
    // replace the existing data with the new data
    if (current->entry->ptr) {
      _free(ctx, current->entry->ptr);
    }

    current->entry->ptr = (void *) _malloc(ctx, sizeof (unsigned char) * size);

    if (current->entry->ptr == NULL) {
      context->error = 1;
      return 0;
    }

    memcpy(current->entry->ptr, value, sizeof (unsigned char) * size);
    current->entry->size = size;
  }

  return 1;
}

static unsigned char delete (void *ctx, unsigned char *key) {
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

  _free(ctx, current->key->key);
  _free(ctx, current->key);
  _free(ctx, current->entry->ptr);
  _free(ctx, current->entry);
  _free(ctx, current);

  return 1;
}

static MemoryKey *find_key (void *ctx, unsigned char *key) {
  MemoryCtx *context = (MemoryCtx *) ctx;
  MemoryKey *current;
  int len = strlen((const char *) key) + 1;

  current = context->head;

  while (current != NULL) {
    if (current->key->size == len) {
      if (memcmp(current->key->key, key, len * sizeof(unsigned char)) == 0) {
        return current;
      }
    } else {
      current = current->next;
    }
  }

  return NULL;
}

Stats *stats (void *ctx) {
  MemoryCtx *context = (MemoryCtx *) ctx;

  return context->stats;
}

static void scan (void *ctx, void (*dataHandler)(unsigned char *, unsigned char *), void (*endHandler)(), void (*errorHandler)(char *)) {
  MemoryCtx *context = (MemoryCtx *) ctx;
  MemoryKey *current;

  current = context->head;
  while (current != NULL) {
    dataHandler(current->key->key, (unsigned char *) current->entry->ptr);

    current = current->next;
  }

  endHandler();
}

static char *last_error (void *ctx) {
  MemoryCtx *context = (MemoryCtx *) ctx;
  int cur = context->error;
  if (cur) {
    context->error = 0;
    return errors[cur];
  } else {
    return NULL;
  }
}

static void *create_context ( ) {
  /*
  // context to run within
  static struct memory_ctx {
    int error;
    MemoryKey *head;
    Stats stats;
  };
  */
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
  free(context->stats);
  free(ctx);
}


static void *_malloc (void *ctx, unsigned int size) {
  MemoryCtx *context = (MemoryCtx *) ctx;
  void *ptr;
  void *new_ptr;
  unsigned int *sz;

  size += sizeof(unsigned int);

  ptr = malloc(size);
  context->stats->memory_usage += size;

  sz = ptr;
  *sz = size;

  new_ptr = ptr + sizeof(unsigned int);

  return new_ptr;
}

static void _free (void *ctx, void *ptr) {
  MemoryCtx *context = (MemoryCtx *) ctx;
  unsigned int *sz;
  void *true_ptr;

  true_ptr = ptr - sizeof(unsigned int);

  sz = true_ptr;

  context->stats->memory_usage -= *sz;

  free(true_ptr);
}
