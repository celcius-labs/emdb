#ifndef __QUERY_H__
#define __QUERY_H__

#include "emdb.h"

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
  ValueType value_type;
  unsigned char child_count;
  void **children;
} Where;

typedef struct QueryResults {
  int count;
  unsigned char *keys[];
} QueryResults;

//QueryResults *emdb_query_db(EMDB *, Where *);
//void emdb_free_results(QueryResults *);

unsigned char compare_float (float, float, WhereType);
unsigned char compare_int (int, int, WhereType);
unsigned char compare_string (char *, char *, WhereType);

#endif /* __QUERY_H__ */
