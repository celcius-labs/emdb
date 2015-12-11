#include <stdlib.h>
#include <string.h>

#include "query.h"

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

unsigned char compare_string (char *value1, char * value2, WhereType operand) {
  int len1 = strlen(value1);
  int len2 = strlen(value2);
  int len = MAX(len1, len2);

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
