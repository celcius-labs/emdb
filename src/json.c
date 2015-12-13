#include "json.h"
#include "emdb.h"
#include "emdb_config.h"

#include "jsmn.h"

#include <string.h>
#include <stdlib.h>

static jsmn_parser p;
unsigned char init = 0;

char **emdb_split_key (char *str) {
  static char *keys[MAX_KEY_DEPTH + 1];
  int i;
  int j = 0;
  int len = strlen(str);
  char *buffer = (char *) malloc(len + 1);

  strcpy(buffer, str);

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

void emdb_free_keys (char **key) {
  free(*key);
}
