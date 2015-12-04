#include <string.h>
#include <stdlib.h>

#include "memory.h"

static MemoryKey *head = NULL;
static MemoryKey *find_key (unsigned char *);

static Entry *read (unsigned char *);
static unsigned char write (unsigned char *, unsigned char *, int);
static unsigned char delete (unsigned char *);
static void init ( );
static Stats *stats ( );


static void *_malloc(unsigned int);
static void _free(void *);

static Stats _stats = { 0, 0 };

Storage MemoryStorage = {
  write,
  read,
  delete,
  init,
  stats
};

static void init ( ) {

}

static Entry *read (unsigned char *key) {
  MemoryKey *current = find_key(key);

  if (current == NULL) {
    return NULL;
  } else {
    return emdb_copy_entry(current->entry);
  }
}

static unsigned char write (unsigned char *key, unsigned char *value, int size) {
  MemoryKey *current = find_key(key);
  int key_size = strlen((const char *) key) + 1;

  if (current == NULL) {
    // new entry, create it, and add it to the list as the head
    current = (MemoryKey *) _malloc(sizeof (MemoryKey));

    // failed to allocate memory
    if (current == NULL) {
      return 0;
    }

    // allocate the memory for the key
    current->key = (Key *) _malloc(sizeof (Key));

    // failed to allocate memory
    if (current->key == NULL) {
      _free(current);
      return 0;
    }

    current->key->key = (unsigned char *) _malloc(sizeof (unsigned char) * key_size);

    // falled to allocate memory
    if (current->key->key == NULL) {
      _free(current->key);
      _free(current);
      return 0;
    }

    current->key->size = key_size;
    memcpy(current->key->key, key, sizeof (unsigned char) * key_size);

    // allocate memory for the Entry
    current->entry = (Entry *) _malloc(sizeof (Entry));

    // failed to allocate memory
    if (current->entry == NULL) {
      _free(current->key->key);
      _free(current->key);
      _free(current);
      return 0;
    }

    // allocate the memory for the data itself
    current->entry->ptr = (void *) _malloc(sizeof (unsigned char) * size);

    // unable to malloc, free up the container and return 0
    if (current->entry->ptr == NULL) {
      _free(current->key->key);
      _free(current->key);
      _free(current->entry);
      _free(current);
      return 0;
    }

    memcpy(current->entry->ptr, value, sizeof(unsigned char) * size);
    current->entry->size = size;
    current->next = head;
    head = current;
  } else {
    // replace the existing data with the new data
    if (current->entry->ptr) {
      _free(current->entry->ptr);
    }

    current->entry->ptr = (void *) _malloc(sizeof (unsigned char) * size);

    if (current->entry->ptr == NULL) {
      return 0;
    }

    memcpy(current->entry->ptr, value, sizeof (unsigned char) * size);
    current->entry->size = size;
  }

  return 1;
}

static unsigned char delete (unsigned char *key) {
  MemoryKey *current = find_key(key);
  MemoryKey *prev;

  if (current == NULL) {
    return 0;
  }

  // find the entry and remove it,
  // if the head, point head to next
  if (current == head) {
    head = current->next;
  } else {
    // find the parent entry
    prev = head;
    while (prev && prev->next != current) {
      prev = prev->next;
    }

    if (prev == NULL) {
      // odd, not found
      return 0;
    }

    prev->next = current->next;
  }

  _free(current->key->key);
  _free(current->key);
  _free(current->entry->ptr);
  _free(current->entry);
  _free(current);

  return 1;
}

static MemoryKey *find_key (unsigned char *key) {
  MemoryKey *current;
  int len = strlen((const char *) key) + 1;

  current = head;
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

Stats *stats ( ) {
  return &_stats;
}

static void *_malloc (unsigned int size) {
  void *ptr;
  void *new_ptr;
  unsigned int *sz;

  size += sizeof(unsigned int);

  ptr = malloc(size);
  _stats.memory_usage += size;

  sz = ptr;
  *sz = size;

  new_ptr = ptr + sizeof(unsigned int);

  return new_ptr;
}

static void _free (void *ptr) {
  unsigned int *sz;
  void *true_ptr;

  true_ptr = ptr - sizeof(unsigned int);

  sz = true_ptr;

  _stats.memory_usage -= *sz;

  free(true_ptr);
}
