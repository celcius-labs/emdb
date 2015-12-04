#ifndef MEMORY_H
#define MEMORY_H

#include "../emdb.h"

Storage MemoryStorage;

typedef struct MemoryKey {
  Key *key;
  Entry *entry;
  struct MemoryKey *next;
} MemoryKey;


#endif /* MEMORY_H */
