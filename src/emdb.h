#ifndef __EMDB_H__
#define __EMDB_H__

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
  unsigned char (*write)(void *, unsigned char *, unsigned char *, int);
  Entry *(*read)(void *, unsigned char *);
  unsigned char (*delete)(void *, unsigned char *);
  void (*scan)(void *, void (*)(unsigned char *, Entry *), void (*)(), void (*)(char *));
  Stats *(*stats)(void *);
  char *(*last_error)(void *);
  void *(*create_context)(void *);
  void (*destroy_context)(void *);
} Storage;

typedef struct {
  unsigned short count;
  unsigned int memory;
  unsigned int max_memory;
  int error;
  Storage *store;
  void *ctx;
} EMDB;


EMDB *emdb_create_db(Storage *, unsigned int, void *);
void emdb_close_db(EMDB *);
unsigned char emdb_write(EMDB *, unsigned char *, unsigned char *, int);
Entry *emdb_read(EMDB *, unsigned char *);
unsigned char emdb_delete(EMDB *, unsigned char *);
char *emdb_last_error(EMDB *);
void emdb_destroy_db(EMDB *);
void emdb_scan(EMDB *, void (*)(unsigned char *, Entry *), void (*)(), void (*)(char *));

void emdb_free_entry(Entry *);
Entry *emdb_copy_entry(Entry *);

#endif /* __EMDB_H__ */
