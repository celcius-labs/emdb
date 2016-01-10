#pragma once
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
