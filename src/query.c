#include <stdlib.h>
#include <string.h>

#include "query.h"
#include "json.h"

#define MAX(a,b) (a > b ? a : b)

unsigned char compare_float (float value1, float value2, WhereType operand) {
  switch (operand) {
    case equals:
    return (value1 == value2);

    case gt:
    return (value1 > value2);

    case lt:
    return (value1 < value2);

    case gte:
    return (value1 >= value2);

    case lte:
    return (value1 <= value2);

    default:
    return 0;
  }
}

unsigned char compare_float_between (float value, float value1, float value2) {
  if (value1 > value2) {
    return (value >= value2 && value <= value1);
  } else {
    return (value >= value1 && value <= value2);
  }
}

unsigned char compare_int (int value1, int value2, WhereType operand) {
  switch (operand) {
    case equals:
    return (value1 == value2);

    case gt:
    return (value1 > value2);

    case lt:
    return (value1 < value2);

    case gte:
    return (value1 >= value2);

    case lte:
    return (value1 <= value2);

    default:
    return 0;
  }
}

unsigned char compare_int_between (int value, int value1, int value2) {
  if (value1 > value2) {
    return (value >= value2 && value <= value1);
  } else {
    return (value >= value1 && value <= value2);
  }
}

uint8_t compare_string (uint8_t *value1, uint8_t *value2, WhereType operand) {
  uint16_t len1 = strlen((char *) value1);
  uint16_t len2 = strlen((char *) value2);
  uint16_t len = MAX(len1, len2);

  switch (operand) {
    case equals:
    if (len1 != len2) {
      return 0;
    }
    return (memcmp(value1, value2, len) == 0);

    case gt:
    return (memcmp(value1, value2, len) > 0);

    case lt:
    return (memcmp(value1, value2, len) < 0);

    case gte:
    return (memcmp(value1, value2, len) >= 0);

    case lte:
    return (memcmp(value1, value2, len) <= 0);

    default:
    return 0;
  }
}

uint8_t compare_string_between (uint8_t *value, uint8_t *value1, uint8_t *value2) {
  int8_t left = strcmp((char *) value, (char *) value1);
  int8_t right = strcmp((char *) value, (char *) value2);

  if (left >= 0 && right <= 0) {
    return 1;
  } else {
    return 0;
  }
}

char **and_list (char **list1, char **list2, int len1, int len2) {
  int i, j, k;
  int max = MAX(len1, len2);
  char **results = (char **) malloc(sizeof(char *) * (max + 1));

  if (results == NULL) {
    return NULL;
  }

  k = 0;

  for (i = 0; i < max; i++) {
    if (len1 == max) {
      for (j = 0; i < len2; j++) {
        if (strcmp(list1[i], list2[j]) == 0) {
          results[k] = list1[i];
          k++;
          break;
        }
      }
    } else {
      for (j = 0; j < len1; j++) {
        if (strcmp(list1[j], list2[i]) == 0) {
          results[k] = list2[i];
          k++;
          break;
        }
      }
    }
  }

  results[k] = NULL;

  return results;
}

char **or_list (char **list1, char **list2, int len1, int len2) {
  int i, j, k;
  unsigned char found = 0;
  char **results = (char **) malloc(sizeof(char *) * (len1 + len2 + 1));

  if (results == NULL) {
    return NULL;
  }

  k = 0;

  for (i = 0; i < len1; i++) {
    results[k] = list1[i];
    k++;
  }

  for (i = 0; i < len2; i++) {
    for (j = 0; j < len1; j++) {
      if (strcmp(list1[j], list2[i]) == 0) {
        found = 1;
        break;
      }
    }

    if (found == 0) {
      results[k] = list2[i];
      k++;
    } else {
      found = 0;
    }
  }

  results[k] = NULL;

  return results;
}

static void simple_query_entry_handler (void *ctx, uint8_t *key, Entry *value) {
  SimpleQueryContext *context = (SimpleQueryContext *) ctx;

  if (context->where->value_type == floatingpoint) {
    float float_res = float_from_json(context->json_ctx, value->ptr, context->where->key);
    uint8_t res = compare_float(float_res, context->where->value.as_float, context->where->type);
    if (context->where->not) {
      if (!res) {
        context->results->keys[context->results->count] = key;
        context->results->count++;
      }
    } else {
      if (res) {
        context->results->keys[context->results->count] = key;
        context->results->count++;
      }
    }
  } else if (context->where->value_type == integer) {
    uint16_t int_res = int_from_json(context->json_ctx, value->ptr, context->where->key);
    uint8_t res = compare_int(int_res, context->where->value.as_int, context->where->type);
    if (context->where->not) {
      if (!res) {
        context->results->keys[context->results->count] = key;
        context->results->count++;
      }
    } else {
      if (res) {
        context->results->keys[context->results->count] = key;
        context->results->count++;
      }
    }
  } else if (context->where->value_type == string) {
    uint8_t *string_res = string_from_json(context->json_ctx, value->ptr, context->where->key);
    uint8_t res = compare_string(string_res, context->where->value.as_char, context->where->type);
    if (context->where->not) {
      if (!res) {
        context->results->keys[context->results->count] = key;
        context->results->count++;
      }
    } else {
      if (res) {
        context->results->keys[context->results->count] = key;
        context->results->count++;
      }
    }
  }

}

void emdb_query_db (EMDB *emdb, Where *where, void (*callback)(QueryResults *)) {
  int i;
  SimpleQueryContext *query_ctx = (SimpleQueryContext *) malloc(sizeof(SimpleQueryContext));

  if (query_ctx == NULL) {
    // unable to allocate memory
    emdb->error = 1;
    callback(NULL);

    return;
  }


  // setup
  query_ctx->emdb = emdb;
  query_ctx->json_ctx = create_json_context();

  if (query_ctx->json_ctx == NULL) {
    // unable to allocate memory
    emdb->error = 1;
    free(query_ctx);

    callback(NULL);
    return;
  }

  query_ctx->results = (QueryResults *) malloc(sizeof(QueryResults));

  if (query_ctx->results == NULL) {
    emdb->error = 1;
    destroy_json_context(query_ctx->json_ctx);
    free(query_ctx);

    callback(NULL);
    return;
  }

  query_ctx->results->keys = (uint8_t **) malloc(sizeof(uint8_t *) * emdb->count);

  if (query_ctx->results->keys == NULL) {
    emdb->error = 1;
    free(query_ctx->results);
    destroy_json_context(query_ctx->json_ctx);
    free(query_ctx);

    callback(NULL);
    return;
  }

  // iterate through the children, getting any results
  if (where->type == and || where->type == or) {
    for (i = 0; i < where->child_count; i++) {

    }
  } else {
    // simple query

  }

}

void emdb_free_results (QueryResults *results) {
  free(results->keys);
  free(results);
}
