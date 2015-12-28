#ifndef __JSON_H__
#define __JSON_H__

#include "emdb_config.h"
#include "jsmn.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct JsonContext {
  jsmn_parser json_parser;
  jsmntok_t tokens[JSON_MAX_TOKENS];
  unsigned char error;
} JsonContext;

char **emdb_split_key (char *);
void emdb_free_keys (char **);

JsonContext *create_json_context ( );
void destroy_json_context (JsonContext *);

int int_from_json (JsonContext *, char *, char *);
float float_from_json (JsonContext *, char *, char *);
char *string_from_json (JsonContext *, char *, char *);

#ifdef __cplusplus
}
#endif

#endif /* __JSON_H__ */
