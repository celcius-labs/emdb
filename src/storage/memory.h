#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "../emdb.h"

#ifdef __cplusplus
extern "C" {
#endif

#define EMDB_MEMORYSTORAGE_VERSION "0.1.0"

Storage MemoryStorage;

typedef struct MemoryKey {
  Key *key;
  Entry *entry;
  struct MemoryKey *next;
} MemoryKey;

#ifdef __cplusplus
}
#endif


#endif /* __MEMORY_H__ */
