#include <stdio.h>

#include "storage/memory.h"
#include "emdb.h"


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
  emdb_write(db, (uint8_t *) "2015-06-29", (uint8_t *) &day1, sizeof(day1));
  emdb_write(db, (uint8_t *) "2015-06-30", (uint8_t *) &day2, sizeof(day2));
}

struct temperatures {
  float min;
  float max;
  float total;
  uint16_t count;
};

void entry_handler (void *ctx, uint8_t *key, Entry *value) {
  DailyTemperature *temp = (DailyTemperature *) value->ptr;
  float day_total = 0, day_max = -1000, day_min = 1000;
  uint16_t i, day_count = 0;

  struct temperatures *t = (struct temperatures *) ctx;

  for (i = 0; i < temp->max_hour; i++) {
    if (temp->current[i] > t->max) {
      t->max = temp->current[i];
    }

    if (temp->current[i] > day_max) {
      day_max = temp->current[i];
    }

    if (temp->current[i] < t->min) {
      t->min = temp->current[i];
    }

    if (temp->current[i] < day_min) {
      day_min = temp->current[i];
    }

    t->total += temp->current[i];
    t->count++;

    day_total += temp->current[i];
    day_count++;
  }

  emdb_free_entry(value);

  printf("%s => min: %f, max: %f, avg: %3.3f\n", (char *) key, day_min, day_max, (day_total / day_count));
}

void end_handler (void *ctx) {
  struct temperatures *t = (struct temperatures *) ctx;
  printf("total => min: %f, max: %f, avg: %3.3f\n", t->min, t->max, (t->total / t->count));
}

void error_handler (void *ctx, uint8_t *error) {
  printf("ERROR: %s\n", (char *) error);
}

int main ( ) {
  EMDB *db;
  struct temperatures t;
  Storage *store = getMemoryStorage();

  // set up some crazy defaults
  t.min = 1000;
  t.max = -1000;
  t.count = 0;
  t.total = 0;

  db = emdb_create_db(store, 1024, NULL);
  add_data(db);

  emdb_scan(db, (void *) &t, entry_handler, end_handler, error_handler);
  return 1;
}
