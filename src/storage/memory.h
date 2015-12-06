#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "../emdb.h"

#define EMDB_MEMORYSTORAGE_VERSION "0.1.0"

Storage MemoryStorage;

typedef struct MemoryKey {
  Key *key;
  Entry *entry;
  struct MemoryKey *next;
} MemoryKey;


#endif /* __MEMORY_H__ */
