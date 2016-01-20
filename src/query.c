#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "query.h"
#include "json.h"

#define MAX(a,b) (a > b ? a : b)
#define MIN(a,b) (a < b ? a : b)

uint8_t compare_float (float value1, float value2, WhereType operand) {
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

uint8_t compare_float_between (float value, float value1, float value2) {
  if (value1 > value2) {
    return (value >= value2 && value <= value1);
  } else {
    return (value >= value1 && value <= value2);
  }
}

uint8_t compare_int (uint16_t value1, uint16_t value2, WhereType operand) {
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

uint8_t compare_int_between (uint16_t value, uint16_t value1, uint16_t value2) {
  if (value1 > value2) {
    return (value >= value2 && value <= value1);
  } else {
    return (value >= value1 && value <= value2);
  }
}

uint8_t compare_string (uint8_t *value1, uint8_t *value2, WhereType operand) {
  uint16_t len1 = strlen((char *) value1);
  uint16_t len2 = strlen((char *) value2);
  uint16_t len = MIN(len1, len2);

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

uint8_t **and_list (uint8_t **list1, uint8_t **list2, uint16_t len1, uint16_t len2) {
  uint16_t i, j, k;
  uint16_t max = MAX(len1, len2);
  uint8_t **results = (uint8_t **) malloc(sizeof(uint8_t *) * (max + 1));

  if (results == NULL) {
    return NULL;
  }

  k = 0;

  for (i = 0; i < max; i++) {
    if (len1 == max) {
      for (j = 0; i < len2; j++) {
        if (strcmp((char *) list1[i], (char *) list2[j]) == 0) {
          results[k] = list1[i];
          k++;
          break;
        }
      }
    } else {
      for (j = 0; j < len1; j++) {
        if (strcmp((char *) list1[j], (char *) list2[i]) == 0) {
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

uint8_t **or_list (uint8_t **list1, uint8_t **list2, uint16_t len1, uint16_t len2) {
  uint16_t i, j, k;
  uint8_t found = 0;
  uint8_t **results = (uint8_t **) malloc(sizeof(uint8_t *) * (len1 + len2 + 1));

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
      if (strcmp((char *) list1[j], (char *) list2[i]) == 0) {
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
    if (string_res == NULL) {
      emdb_free_entry(value);

      return;
    }

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

    free(string_res);
  }

  emdb_free_entry(value);
}

static void simple_end_handler (void *ctx) {
  SimpleQueryContext *context = (SimpleQueryContext *) ctx;
  void (*callback)(QueryResults *) = context->callback;
  QueryResults *results = context->results;

  destroy_simple_query_context(ctx);

  callback(results);
}

static void simple_error_handler (void *ctx, uint8_t *error) {
  SimpleQueryContext *context = (SimpleQueryContext *) ctx;
  EMDB *emdb = context->emdb;
  void (*callback)(QueryResults *) = context->callback;

  emdb->error = 3;


  callback(NULL);
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
  query_ctx->where = where;
  query_ctx->callback = callback;
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

  query_ctx->results->count = 0;
  query_ctx->results->keys = (uint8_t **) malloc(sizeof(uint8_t **) * emdb->count);

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
    // complex queries not yet supported
    assert(0);
    for (i = 0; i < where->child_count; i++) {

    }
  } else {
    // simple query
    emdb_scan(emdb, (void *) query_ctx, simple_query_entry_handler, simple_end_handler, simple_error_handler);
  }

}

void emdb_free_results (QueryResults *results) {
  free(results->keys);
  free(results);
}

void destroy_simple_query_context (SimpleQueryContext *ctx) {
  destroy_json_context(ctx->json_ctx);

  // does not free there Where clause
  // nor the query results
  free(ctx);
}
