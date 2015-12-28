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

unsigned char compare_string_between (char *value, char *value1, char *value2) {
  int left = strcmp(value, value1);
  int right = strcmp(value, value2);

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
