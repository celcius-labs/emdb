#pragma once

#include "emdb_config.h"
#include "jsmn.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct JsonContext {
  jsmn_parser json_parser;
  jsmntok_t tokens[JSON_MAX_TOKENS];
  uint8_t error;
} JsonContext;

uint8_t **emdb_split_key (uint8_t *);
void emdb_free_keys (uint8_t **);

JsonContext *create_json_context ( );
void destroy_json_context (JsonContext *);

uint16_t int_from_json (JsonContext *, uint8_t *, uint8_t *);
float float_from_json (JsonContext *, uint8_t *, uint8_t *);
uint8_t *string_from_json (JsonContext *, uint8_t *, uint8_t *);

#ifdef __cplusplus
}
#endif
