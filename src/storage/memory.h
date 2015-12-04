#ifndef MEMORY_H
#define MEMORY_H

#include "../emdb.h"

Storage MemoryStorage;

typedef struct MemoryKey {
  Key *key;
  Entry *entry;
  struct MemoryKey *next;
} MemoryKey;

static MemoryKey *find_key (unsigned char *);

static Entry *read (unsigned char *);
static unsigned char write (unsigned char *, unsigned char *, int);
static unsigned char delete (unsigned char *);
static void init ( );

#endif /* MEMORY_H */
