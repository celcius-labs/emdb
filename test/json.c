#include "json.h"
#include "test.h"

#include <string.h>
#include <stdio.h>

int test_json_utils ( ) {
  char *string1 = "foo";
  char *string2 = "foo.bar";
  char *string3 = "foo.bar.baz";
  char **parts;

  parts = emdb_split_key(string1);
  check((strcmp(parts[0], "foo") == 0), "part 1 is correct for foo");
  check(parts[1] == NULL, "part 2 is null for foo");

  emdb_free_keys(parts);

  parts = emdb_split_key(string2);
  check((strcmp(parts[0], "foo") == 0), "part 1 is correct for foo");
  check((strcmp(parts[1], "bar") == 0), "part 2 is correct for bar");
  check(parts[2] == NULL, "part 3 is null for foo.bar");

  emdb_free_keys(parts);

  parts = emdb_split_key(string3);
  check((strcmp(parts[0], "foo") == 0), "part 1 is correct for foo");
  check((strcmp(parts[1], "bar") == 0), "part 2 is correct for bar");
  check((strcmp(parts[2], "baz") == 0), "part 3 is correct for baz");
  check(parts[3] == NULL, "part 4 is null for foo.bar");

  emdb_free_keys(parts);

  done();
}

int test_int_from_json ( ) {
  char json[] = "{ \"foo\": { \"bar\": 1 }, \"grr\": [1, 2, 3], \"baz\": \"2\" }";
  char *parts = "foo.bar";
  char *parts2 = "baz";
  JsonContext *ctx;

  ctx = create_json_context();

  check((int_from_json(ctx, json, parts) == 1), "int_from_json returns correct value for nested keys");
  check((int_from_json(ctx, json, parts2) == 2), "int_from_json returns correct value for normal keys");

  check((int_from_json(ctx, json, "whee") == 0), "int_from_json returns 0 for unknown keys");
  check((ctx->error == 3), "and error is set to 3");


  destroy_json_context(ctx);
  done();
}

int test_float_from_json ( ) {
  char json[] = "{ \"foo\": { \"bar\": 1.0 }, \"grr\": [1, 2, 3], \"baz\": \"2.0\" }";
  char *parts = "foo.bar";
  char *parts2 = "baz";
  JsonContext *ctx;

  ctx = create_json_context();

  check((float_from_json(ctx, json, parts) == 1), "float_from_json returns correct value for nested keys");
  check((float_from_json(ctx, json, parts2) == 2), "float_from_json returns correct value for normal keys");

  check((float_from_json(ctx, json, "whee") == 0), "float_from_json returns 0 for unknown keys");
  check((ctx->error == 3), "and error is set to 3");


  destroy_json_context(ctx);
  done();
}
