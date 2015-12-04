#ifndef EMDB_H
#define EMDB_H

#include <limits.h>

typedef struct Entry {
  int size;
  void *ptr;
} Entry;

typedef struct Key {
  int size;
  unsigned char *key;
} Key;

typedef struct Stats {
  unsigned int memory_usage;
  unsigned int entries;
} Stats;


typedef struct {
  unsigned char (*write)(unsigned char *, unsigned char *, int);
  Entry *(*read)(unsigned char *);
  unsigned char (*delete)(unsigned char *);
  void (*init)();
  Stats *(*stats)();

  /*
  byte write(unsigned char *key, unsigned char *value)
  unsigned char *read(unsigned char *key)
  byte delete(unsgined char *key)
  void scan(dataHandler, endHandler, errorHandler)
  unsigned int memory()
  */
} Storage;

typedef struct {
  unsigned short count;
  unsigned int memory;
  unsigned int max_memory;
  Storage *store;
} EMDB;


EMDB *emdb_create_db(Storage *, unsigned int max_memory);
void emdb_close_db(EMDB *);
unsigned char emdb_write(EMDB *, unsigned char *, unsigned char *, int);

Entry *emdb_copy_entry(Entry *);

#endif /* EMDB_H */
