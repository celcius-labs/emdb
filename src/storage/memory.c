#include <string.h>
#include <stdlib.h>

#include "memory.h"

static MemoryKey *head = NULL;

Storage MemoryStorage = {
  write,
  read,
  delete,
  init
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
    current = (MemoryKey *) malloc(sizeof (MemoryKey));

    // failed to allocate memory
    if (current == NULL) {
      return 0;
    }

    // allocate the memory for the key
    current->key = (Key *) malloc(sizeof (Key));

    // failed to allocate memory
    if (current->key == NULL) {
      free(current);
      return 0;
    }

    current->key->key = (unsigned char *) malloc(sizeof (unsigned char) * key_size);

    // falled to allocate memory
    if (current->key->key == NULL) {
      free(current->key);
      free(current);
      return 0;
    }

    current->key->size = key_size;
    memcpy(current->key->key, key, sizeof (unsigned char) * key_size);

    // allocate memory for the Entry
    current->entry = (Entry *) malloc(sizeof (Entry));

    // failed to allocate memory
    if (current->entry == NULL) {
      free(current->key->key);
      free(current->key);
      free(current);
      return 0;
    }

    // allocate the memory for the data itself
    current->entry->ptr = (void *) malloc(sizeof (unsigned char) * size);

    // unable to malloc, free up the container and return 0
    if (current->entry->ptr == NULL) {
      free(current->key->key);
      free(current->key);
      free(current->entry);
      free(current);
      return 0;
    }

    memcpy(current->entry->ptr, value, sizeof(unsigned char) * size);
    current->entry->size = size;
    current->next = head;
    head = current;
  } else {
    // replace the existing data with the new data
    if (current->entry->ptr) {
      free(current->entry->ptr);
    }

    current->entry->ptr = (void *) malloc(sizeof (unsigned char) * size);

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

  free(current->key->key);
  free(current->key);
  free(current->entry->ptr);
  free(current->entry);
  free(current);

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
