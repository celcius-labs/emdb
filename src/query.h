#ifndef __QUERY_H__
#define __QUERY_H__

#include "emdb.h"

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
  and     = 0,
  or      = 1,
  equals  = 2,
  gt      = 3,
  lt      = 4,
  gte     = 5,
  lte     = 6,
  between = 7,
  in      = 8
} WhereType;

typedef enum ValueType {
  string,
  integer,
  floatingpoint
} ValueType;

typedef struct Where {
  WhereType type;
  unsigned char *key;
  unsigned char *value;
  unsigned char not;
  ValueType value_type;
  unsigned char child_count;
  void **children;
} Where;

typedef struct QueryResults {
  int count;
  unsigned char *keys[];
} QueryResults;

QueryResults *emdb_query_db(EMDB *, Where *);
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
unsigned char compare_float (float, float, WhereType);

/**
 * @brief Compares two integers.
 *
 * Compares the value of two integers for equality, less than, greater than,
 * less than or equal, or greater than or equal.
 * @returns true or false
 * @param int1 - int
 * @param int2 - int
 * @param type - type of match to do
 * @see WhereType
 */
unsigned char compare_int (int, int, WhereType);

/**
 * @brief Compares two strings.
 *
 * Compares the value of two strings for equality, less than, greater than,
 * less than or equal, or greater than or equal.
 * @returns true or false
 * @param string1 - char *
 * @param string2 - char *
 * @param type - type of match to do
 * @see WhereType
 */
unsigned char compare_string (char *, char *, WhereType);

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
unsigned char compare_float_between (float, float, float);

/**
 * @brief Checks to see if an integer is between two other integers, inclusively.
 *
 * Compares the value of an integer to two other integers, to see if it is between
 * them, or equals them
 * @returns true or false
 * @param integer - integer to check
 * @param minimum - integer marking the minimum
 * @param maximum - integer marking the maximum
 */
unsigned char compare_int_between (int, int, int);

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
unsigned char compare_string_between (char *, char *, char *);

/**
 * @brief Compares two lists and returns all entries that are in both lists.
 *
 * Iterates through two lists, and returns every entry that is in both lists,
 * NULL terminated.  The results must be cleaned up by calling free(), as this
 * will allocate memory.
 * @returns char ** - allocated array of strings that points to the list of the
 * combined lists.
 * @param list1 - char ** first list
 * @param list2 - char ** second list
 * @param len1 - length of list 1
 * @param len2 - length of list 2
 */
char **and_list (char **, char **, int, int);

/**
 * @brief Compares two lists and returns all entries of the combined lists.
 *
 * Iterates through two lists, and returns every entry in each list,
 * NULL terminated.  The results must be cleaned up by calling free(), as this
 * will allocate memory.
 * @returns char ** - allocated array of strings that points to the list of the
 * combined lists.
 * @param list1 - char ** first list
 * @param list2 - char ** second list
 * @param len1 - length of list 1
 * @param len2 - length of list 2
 */
char **or_list (char **, char **, int, int);

#ifdef __cplusplus
}
#endif

#endif /* __QUERY_H__ */
