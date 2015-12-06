#include <stdio.h>

#include "storage/memory.h"
#include "emdb.h"

//void (*scan)(void *, void (*)(unsigned char *, unsigned char *), void (*)(), void (*)(char *));

typedef struct {
  float current[24];
  int max_hour;
} DailyTemperature;

DailyTemperature day1 = {
  { 62, 63.2, 63, 64, 64.2, 70, 71, 73, 75, 75.2, 76.4 },
  11
};

DailyTemperature day2 = {
  { 66, 65.3, 62.1, 60, 62.2, 68.3, 71, 68.5, 67.21, 66.2, 63.4 },
  11
};

void add_data (EMDB *db) {
  emdb_write(db, (unsigned char *) "2015-06-29", (unsigned char *) &day1, sizeof(day1));
  emdb_write(db, (unsigned char *) "2015-06-30", (unsigned char *) &day2, sizeof(day2));
}

float min = 1000, max = -1000, avg, total = 0;
int count = 0;

void entry_handler (unsigned char *key, Entry *value) {
  DailyTemperature *temp = (DailyTemperature *) value->ptr;
  float day_total = 0, day_max = -1000, day_min = 1000;
  int i, day_count = 0;

  for (i = 0; i < temp->max_hour; i++) {
    if (temp->current[i] > max) {
      max = temp->current[i];
    }

    if (temp->current[i] > day_max) {
      day_max = temp->current[i];
    }

    if (temp->current[i] < min) {
      min = temp->current[i];
    }

    if (temp->current[i] < day_min) {
      day_min = temp->current[i];
    }

    total += temp->current[i];
    count++;

    day_total += temp->current[i];
    day_count++;
  }

  emdb_free_entry(value);

  printf("%s => min: %f, max: %f, avg: %3.3f\n", (char *) key, day_min, day_max, (day_total / day_count));
}

void end_handler ( ) {
  printf("total => min: %f, max: %f, avg: %3.3f\n", min, max, (total / count));
}

void error_handler (char *error) {
  printf("ERROR: %s\n", error);
}

int main ( ) {
  EMDB *db;

  db = emdb_create_db(&MemoryStorage, 1024, NULL);
  add_data(db);

  emdb_scan(db, entry_handler, end_handler, error_handler);
  return 1;
}
