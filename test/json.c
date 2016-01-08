#include "json.h"
#include "test.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t test_json_utils ( ) {
  uint8_t *string1 = (uint8_t *) "foo";
  uint8_t *string2 = (uint8_t *) "foo.bar";
  uint8_t *string3 = (uint8_t *) "foo.bar.baz";
  uint8_t **parts;

  parts = emdb_split_key((uint8_t *) string1);
  check((strcmp((char *) parts[0], "foo") == 0), "part 1 is correct for foo");
  check(parts[1] == NULL, "part 2 is null for foo");

  emdb_free_keys(parts);

  parts = emdb_split_key(string2);
  check((strcmp((char *) parts[0], "foo") == 0), "part 1 is correct for foo");
  check((strcmp((char *) parts[1], "bar") == 0), "part 2 is correct for bar");
  check(parts[2] == NULL, "part 3 is null for foo.bar");

  emdb_free_keys(parts);

  parts = emdb_split_key(string3);
  check((strcmp((char *) parts[0], "foo") == 0), "part 1 is correct for foo");
  check((strcmp((char *) parts[1], "bar") == 0), "part 2 is correct for bar");
  check((strcmp((char *) parts[2], "baz") == 0), "part 3 is correct for baz");
  check(parts[3] == NULL, "part 4 is null for foo.bar");

  emdb_free_keys(parts);

  done();
}

uint8_t test_int_from_json ( ) {
  uint8_t json[] = "{ \"foo\": { \"bar\": 1 }, \"grr\": [1, 2, 3], \"baz\": \"2\" }";
  uint8_t *parts = (uint8_t *) "foo.bar";
  uint8_t *parts2 = (uint8_t *) "baz";
  JsonContext *ctx;

  ctx = create_json_context();

  check((int_from_json(ctx, json, parts) == 1), "int_from_json returns correct value for nested keys");
  check((int_from_json(ctx, json, parts2) == 2), "int_from_json returns correct value for normal keys");

  check((int_from_json(ctx, json, (uint8_t *) "whee") == 0), "int_from_json returns 0 for unknown keys");
  check((ctx->error == 3), "and error is set to 3");


  destroy_json_context(ctx);
  done();
}

uint8_t test_float_from_json ( ) {
  uint8_t json[] = "{ \"foo\": { \"bar\": 1.0 }, \"grr\": [1, 2, 3], \"baz\": \"2.0\" }";
  uint8_t *parts = (uint8_t *) "foo.bar";
  uint8_t *parts2 = (uint8_t *) "baz";
  JsonContext *ctx;

  ctx = create_json_context();

  check((float_from_json(ctx, json, parts) == 1), "float_from_json returns correct value for nested keys");
  check((float_from_json(ctx, json, parts2) == 2), "float_from_json returns correct value for normal keys");

  check((float_from_json(ctx, json, (uint8_t *) "whee") == 0), "float_from_json returns 0 for unknown keys");
  check((ctx->error == 3), "and error is set to 3");


  destroy_json_context(ctx);
  done();
}

uint8_t test_string_from_json ( ) {
  uint8_t json[] = "{ \"foo\": { \"bar\": \"whee\" }, \"grr\": [1, 2, 3], \"baz\": \"2.0\" }";
  uint8_t *parts = (uint8_t *) "foo.bar";
  uint8_t *parts2 = (uint8_t *) "baz";
  uint8_t *buf;
  JsonContext *ctx;

  ctx = create_json_context();

  buf = string_from_json(ctx, json, parts);
  check(strcmp((char *) buf, "whee") == 0, "string_from_json returns correct value for nested keys");
  free(buf);

  buf = string_from_json(ctx, json, parts2);
  check(strcmp((char *) buf, "2.0") == 0, "string_from_json returns correct value for normal keys");
  free(buf);

  check((string_from_json(ctx, json, (uint8_t *) "whee") == NULL), "string_from_json returns NULL for unknown keys");
  check((ctx->error == 3), "and error is set to 3");

  destroy_json_context(ctx);
  done();
}
