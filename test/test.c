#include <stdio.h>
#include <string.h>

#ifdef EMDB_MEMORY_STORAGE
#include "storage/memory.h"
#endif

#include "emdb.h"


#include "test.h"

uint8_t spec = 0;
uint16_t test_passed = 0;
uint16_t test_failed = 0;

uint8_t test_emdb ( ) {
  EMDB *db;
  Entry *entry;
  uint8_t ret;

  db = emdb_create_db(&MemoryStorage, 1024, NULL);

  check(db != NULL, "database is created");

  entry = emdb_read(db, (unsigned char *) "foo");

  check(entry == NULL, "entry is null after read");

  ret = emdb_write(db, (unsigned char *) "foo", (unsigned char *) "bar", 4);
  check(ret == 1, "write is successful");

  entry = emdb_read(db, (unsigned char *) "foo");

  check(entry->size == 4, "entry size is 4");
  check(strcmp(entry->ptr, "bar") == 0, "entry is correct");

  emdb_free_entry(entry);

  emdb_destroy_db(db);

  done();
}

int main (int argc, char **argv) {
  if (argc > 1) {
    if ((strcmp(argv[1], "--spec") == 0) || (strcmp(argv[1], "-s") == 0)) {
      spec = 1;
    }
  }

  test(test_emdb, "test emdb");

#ifdef EMDB_MEMORY_STORAGE
  test(test_memory, "memory storage");
  test(test_context_isolation, "context isolation");
#endif

#ifdef EMDB_QUERY
  test(test_compare_string, "comparison string");
  test(test_compare_int, "comparison int");
  test(test_compare_float, "comparison float");
  test(test_and_list, "and_list");
  test(test_or_list, "or_list");
#endif

#ifdef EMDB_JSON
  test(test_json_utils, "json utilities");
  test(test_int_from_json, "int_from_json");
  test(test_float_from_json, "float_from_json");
  test(test_string_from_json, "string_from_json");

#endif

  printf("\nPASSED: %d\nFAILED: %d\n", test_passed, test_failed);

  return (test_failed > 0);
}
