#include "json.h"
#include "test.h"

#include <string.h>
#include <stdio.h>

int test_json_utils ( ) {
  char *string1 = "foo";
  char *string2 = "foo.bar";
  char *string3 = "foo.bar.baz";
  char **parts;
  int c;

  parts = emdb_split_key(string1);
  check((strcmp(parts[0], "foo") == 0), "part 1 is correct for foo");
  check(parts[1] == NULL, "part 2 is null for foo");

  emdb_free_keys(parts);

  parts = emdb_split_key(string2);
  check((strcmp(parts[0], "foo") == 0), "part 1 is correct for foo");
  check((strcmp(parts[1], "bar") == 0), "part 2 is correct for bar");
  check(parts[2] == NULL, "part 3 is null for foo.bar");

  emdb_free_keys(parts);

  done();
}
