#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "../emdb.h"

Storage MemoryStorage;

typedef struct MemoryKey {
  Key *key;
  Entry *entry;
  struct MemoryKey *next;
} MemoryKey;


#endif /* __MEMORY_H__ */
