#include "json.h"
#include "emdb.h"
#include "query.h"
#include "emdb_config.h"

#include "jsmn.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static uint8_t *errors[] = {
  NULL,
  (uint8_t *) "Unable to parse JSON",
  (uint8_t *) "Expected Object",
  (uint8_t *) "Value not found",
  (uint8_t *) "Unable to allocate memory"
};

uint8_t **emdb_split_key (uint8_t *str) {
  uint8_t **keys;
  uint16_t i;
  uint16_t j = 0;
  uint16_t len = strlen((char *) str);
  uint8_t *buffer;

  keys = (uint8_t **) malloc(sizeof(uint8_t *) * MAX_KEY_DEPTH);

  if (keys == NULL) {
    return NULL;
  }

  // make a copy of the string that we can modify
  buffer = (uint8_t *) malloc(len + 1);

  if (buffer == NULL) {
    free(keys);
    return NULL;
  }

  strcpy((char *) buffer, (char *) str);

  // set the first key to the address of the string copy
  keys[j] = buffer;
  j++;

  // start at 1, there at least needs to be a first character
  for (i = 1; i < len; i++) {
    if (buffer[i] == '.') {
      buffer[i] = '\0';
      i++;
      if (i < len) {
        keys[j] = &buffer[i];
        j++;
      }
    }
  }

  keys[j] = NULL;

  return keys;
}

void emdb_free_keys (uint8_t **key) {
  free(*key);
  free(key);
}

JsonContext *create_json_context ( ) {
  JsonContext *ctx;

  ctx = (JsonContext *) malloc(sizeof(JsonContext));

  if (ctx == NULL) {
    return NULL;
  }

  jsmn_init(&ctx->json_parser);
  ctx->error = 0;

  return ctx;
}

void destroy_json_context (JsonContext *ctx) {
  free(ctx);
}

uint8_t *get_type (uint8_t type) {
  switch (type) {
    case JSMN_OBJECT:
      return (uint8_t *) "Object";
    case JSMN_ARRAY:
      return (uint8_t *) "Array";
    case JSMN_STRING:
      return (uint8_t *) "String";
    case JSMN_PRIMITIVE:
      return (uint8_t *) "Primitive";
  }

  return (uint8_t *) "Unknown";
}

static int16_t token_from_json (uint8_t *json, jsmntok_t *tokens, uint16_t ntokens, uint16_t *curtoken, uint8_t *parts[], uint16_t *curpart, uint16_t nparts) {
  uint16_t len, i, start, end;
  uint8_t buf[128] = {0};

#ifdef DEBUG
  printf("token_from_json: curtoken is %d, ntokens is %d\n", *curtoken, ntokens);
#endif

  // if we reach the end, return -1 and allow to unwind
  if (*curtoken == ntokens || *curpart == nparts) {
    return -1;
  }

#ifdef DEBUG
  printf("currently looking for %s\n", parts[*curpart]);
#endif

  // if the current token is an object, increment by 1
  // (it should always be an object)
  if (tokens[*curtoken].type != JSMN_OBJECT) {
#ifdef DEBUG
    printf("token %d expected to be an object, instead was of type %s (%d)\n", *curtoken, get_type(tokens[*curtoken].type), tokens[*curtoken].type);
#endif
    return -1;
  }

  len = tokens[*curtoken].size;
  (*curtoken)++;
#ifdef DEBUG
  printf("curtoken is now %d, length is %d\n", *curtoken, len);
#endif

  for (i = 0; i <= len; i++) {
#ifdef DEBUG
    printf("in loop, i = %d, len = %d, curtoken = %d\n", i, len, (*curtoken + i));
#endif
    start = tokens[*curtoken + i].start;
    end = tokens[*curtoken + i].end;
printf("%s %s %d %d\n", buf, &json[start], end, start);
    strncpy((char *) buf, (char *) &json[start], end - start);
    buf[end - start] = '\0';
#ifdef DEBUG
    printf("checking token %d against part %d (%s => %s)\n", *curtoken + i, *curpart, parts[*curpart], buf);
#endif

    if (strcmp((char *) parts[*curpart], (char *) buf) == 0) {
#ifdef DEBUG
      printf("found %s\n", parts[*curpart]);
#endif

      // at the end of the parts
      if (*curpart == nparts - 1) {
#ifdef DEBUG
        printf("at the end of the parts, returning %d\n", *curtoken + i + 1);
#endif
        return *curtoken + i + 1;
      } else {
#ifdef DEBUG
        printf("calling recursively\n");
#endif
        *curtoken = *curtoken + i + 1;
        (*curpart)++;

        return token_from_json (json, tokens, ntokens, curtoken, parts, curpart, nparts);
      }
    } else {
      // if the second half is an object or array, increment curtoken by that length
      if (tokens[*curtoken + i + 1].type == JSMN_OBJECT) {
#ifdef DEBUG
        printf("next token (%d) is of type object, skipping %d\n", *curtoken + i + 1, (tokens[*curtoken + i + 1]. size * 2));
#endif
        *curtoken = *curtoken + (tokens[*curtoken + i + 1].size * 2);
      } else if (tokens[*curtoken + i + 1].type == JSMN_ARRAY) {
#ifdef DEBUG
        printf("next token (%d) is of type array, skipping %d\n", *curtoken + i + 1, (tokens[*curtoken + i + 1]. size));
#endif
        *curtoken = *curtoken + (tokens[*curtoken + i + 1].size);
      }

      *curtoken = *curtoken + 1;
    }
  }

  return -1;
}

static uint16_t key_depth (uint8_t **parts) {
  uint16_t i, nkeys = 0;

  for (i = 0; i < MAX_KEY_DEPTH; i++) {
    if (parts[i] == NULL) {
      nkeys = i;
      break;
    }
  }

  return nkeys;
}

static int16_t _ctx_token_from_json (JsonContext *ctx, uint8_t *json, uint8_t *key) {
  uint8_t **parts = emdb_split_key(key);
  uint16_t nkeys = key_depth(parts);
  uint16_t curtoken = 0, curpart = 0, ntokens;
  int16_t ret;

  ntokens = jsmn_parse(&ctx->json_parser, (char *) json, strlen((char *) json), ctx->tokens, JSON_MAX_TOKENS);

  if (ntokens < 0) {
    ctx->error = 1;

    if (parts) {
      emdb_free_keys(parts);
    }

    return -1;
  }

  ret = token_from_json (json, ctx->tokens, ntokens, &curtoken, parts, &curpart, nkeys);

  // free up the key parts at this point, they are no longer needed
  emdb_free_keys(parts);

  // -1 denotes not found, which should allow us to set an error on the context
  // this is non-optimal, but internal
  // it is up to the calling function to sort it out
  if (ret == -1) {
    ctx->error = 3;

    return -1;
  }

  return ret;
}

uint16_t int_from_json (JsonContext *ctx, uint8_t *json, uint8_t *key) {
  uint16_t start, end;
  uint8_t buf[16];

  int16_t ret = _ctx_token_from_json(ctx, json, key);

  if (ret == -1) {
    return 0;
  }

  // make a copy of the value to convert into an integer for return
  start = ctx->tokens[ret].start;
  end = ctx->tokens[ret].end;

  strncpy((char *) buf, (char *) &json[start], (end - start) < 16 ? (end - start) : 15);
  buf[(end - start) < 16 ? (end - start) : 15] = '\0';

  return atoi((char *) buf);
}

float float_from_json (JsonContext *ctx, uint8_t *json, uint8_t *key) {
  uint16_t start, end;
  uint8_t buf[16];

  int16_t ret = _ctx_token_from_json(ctx, json, key);

  if (ret == -1) {
    return 0;
  }

  // make a copy of the value to convert into an integer for return
  start = ctx->tokens[ret].start;
  end = ctx->tokens[ret].end;

  strncpy((char *) buf, (char *) &json[start], (end - start) < 16 ? (end - start) : 15);
  buf[(end - start) < 16 ? (end - start) : 15] = '\0';

  return atof((char *) buf);
}

uint8_t *string_from_json (JsonContext *ctx, uint8_t *json, uint8_t *key) {
  uint16_t start, end;
  uint8_t *buf;

  int16_t ret = _ctx_token_from_json(ctx, json, key);

  if (ret == -1) {
    return NULL;
  }

  // make a copy of the value to convert into an integer for return
  start = ctx->tokens[ret].start;
  end = ctx->tokens[ret].end;

  buf = (uint8_t *) malloc((end - start) + 1);

  if (buf == NULL) {
    ctx->error = 4;

    return NULL;
  }

  strncpy((char *) buf, (char *) &json[start], (end - start) + 1);
  buf[(end - start) < 16 ? (end - start) : 15] = '\0';

  return buf;
}

uint8_t *json_last_error (JsonContext *ctx) {
  uint8_t last_error = ctx->error;
  if (last_error) {
    ctx->error = 0;
    return errors[last_error];
  } else {
    return NULL;
  }
}
