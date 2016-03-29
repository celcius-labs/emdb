#pragma once

#include "emdb.h"
#include "json.h"

/**
 * @file query.h
 * @brief Header for Query Functions
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type of Equality
 *
 * Type of equality to check in comparisons.
 */
typedef enum WhereType {
  query_and     = 0,
  query_or      = 1,
  query_equals  = 2,
  query_gt      = 3,
  query_lt      = 4,
  query_gte     = 5,
  query_lte     = 6,
  query_between = 7,
  query_in      = 8
} WhereType;

typedef enum ValueType {
  string,
  integer,
  floatingpoint
} ValueType;

typedef struct Where {
  WhereType type;
  uint8_t *key;
  union {
    uint8_t *as_char;
    int16_t as_int;
    float as_float;
  } value;
  uint8_t query_not;
  ValueType value_type;
  uint8_t child_count;
  void **children;
} Where;

typedef struct QueryResults {
  uint16_t count;
  uint8_t **keys;
} QueryResults;

typedef struct SimpleQueryContext {
  QueryResults *results;
  EMDB *emdb;
  Where *where;
  JsonContext *json_ctx;
  void (*callback)(QueryResults *);
} SimpleQueryContext;

void emdb_query_db(EMDB *, Where *, void (*)(QueryResults *));
void emdb_free_results(QueryResults *);

/**
 * @brief Compares two floats.
 *
 * Compares the value of two floats for equality, less than, greater than,
 * less than or equal, or greater than or equal.
 * @returns true or false
 * @param float1 - float
 * @param float2 - float
 * @param type - type of match to do
 * @see WhereType
 */
uint8_t compare_float (float, float, WhereType);

/**
 * @brief Compares two integers.
 *
 * Compares the value of two integers for equality, less than, greater than,
 * less than or equal, or greater than or equal.
 * @returns true or false
 * @param int1 - uint16_t
 * @param int2 - uint16_t
 * @param type - type of match to do
 * @see WhereType
 */
uint8_t compare_int (uint16_t, uint16_t, WhereType);

/**
 * @brief Compares two strings.
 *
 * Compares the value of two strings for equality, less than, greater than,
 * less than or equal, or greater than or equal.
 * @returns true or false
 * @param string1 - uint8_t *
 * @param string2 - uint8_t *
 * @param type - type of match to do
 * @see WhereType
 */
uint8_t compare_string (uint8_t *, uint8_t *, WhereType);

/**
 * @brief Checks to see if a float is between two other floats, inclusively.
 *
 * Compares the value of a float to two other floats, to see if it is between
 * them, or equals them
 * @returns true or false
 * @param float - float to check
 * @param minimum - float marking the minimum
 * @param maximum - float marking the maximum
 */
uint8_t compare_float_between (float, float, float);

/**
 * @brief Checks to see if an integer is between two other integers, inclusively.
 *
 * Compares the value of an integer to two other integers, to see if it is between
 * them, or equals them
 * @returns true or false
 * @param integer - uint16_t to check
 * @param minimum - uint16_t marking the minimum
 * @param maximum - uint16_t marking the maximum
 */
uint8_t compare_int_between (uint16_t, uint16_t, uint16_t);

/**
 * @brief Checks to see if a string is between two other strings, inclusively.
 *
 * Compares the value of a string to two other strings, to see if it is between
 * them, or equals them
 * @returns true or false
 * @param string - string to check
 * @param minimum - string marking the minimum
 * @param maximum - string marking the maximum
 */
uint8_t compare_string_between (uint8_t *, uint8_t *, uint8_t *);

/**
 * @brief Compares two lists and returns all entries that are in both lists.
 *
 * Iterates through two lists, and returns every entry that is in both lists,
 * NULL terminated.  The results must be cleaned up by calling free(), as this
 * will allocate memory.
 * @returns uint8_t ** - allocated array of strings that points to the list of the
 * combined lists.
 * @param list1 - uint8_t ** first list
 * @param list2 - uint8_t ** second list
 * @param len1 - length of list 1
 * @param len2 - length of list 2
 */
uint8_t **and_list (uint8_t **, uint8_t **, uint16_t, uint16_t);

/**
 * @brief Compares two lists and returns all entries of the combined lists.
 *
 * Iterates through two lists, and returns every entry in each list,
 * NULL terminated.  The results must be cleaned up by calling free(), as this
 * will allocate memory.
 * @returns uint8_t ** - allocated array of strings that points to the list of the
 * combined lists.
 * @param list1 - uint8_t ** first list
 * @param list2 - uint8_t ** second list
 * @param len1 - length of list 1
 * @param len2 - length of list 2
 */
uint8_t **or_list (uint8_t **, uint8_t **, uint16_t, uint16_t);

void destroy_simple_query_context (SimpleQueryContext *);

#ifdef __cplusplus
}
#endif
