# EMbeDB

A Micro Key/Value Store with Query Support.

## Features

* Small
* Fast
* Memory Storage
* Disk Storage (coming soon)
* Querying of Data (in progress)
* Native JSON Support (in progress)

## Platforms

The following platforms are being targeted:
* Linux (X86_64, ARM, MIPS)
* OSX - development
* Arduino
* ESP8266

## Development Goals

As small devices become both more powerful and less expensive, it starts to make
sense to move processing and storage toward the edge.  Having a micro database
available where sensors and data exist adds a lot of flexibility and power to
tiny devices.

Among these are to make devices be able to be queried for some historic data
in one operating mode, but also to allow a device to remain disconnected from a
network for a longer period of time, allowing for a device to remain in a lower
power state at the expense of less up to date data.

### TODO (Lots)

* Speed optimized memory storage
* Indexing
* Builds


## Usage and Development

This project is in rapid development, but attempts to be well tested.

All attempts will be made to follow SEMVER versioning requirements.

### Compiling

Running `make` should be enough to get tests built and run:

```shell
$ make
cc -c -I./src -Wall -g test/test.c -o test/test.o
cc -c -I./src -Wall -g test/memory.c -o test/memory.o
cc -c -I./src -Wall -g src/emdb.c -o obj/emdb.o
cc -c -I./src -Wall -g src/storage/memory.c -o obj/storage/memory.o
cc -g test/test.o test/memory.o obj/emdb.o obj/storage/memory.o -o test_runner
./test_runner --spec

test memory storage
  ✓ entry is null after read
  ✓ write succeeded
  ✓ entry size is 4
  ✓ entry is correct
  ✓ memory usage is correctly reported
  ✓ second write succeeded
  ✓ entry size is 4
  ✓ entry is correct
  ✓ entry is null after delete called
  ✓ memory usage is correctly reported

test context isolation
  ✓ write to context 1 succeeded
  ✓ entry size is 4
  ✓ entry is correct
  ✓ memory usage is correctly reported for context 1
  ✓ memory usage is correctly reported for context 2
  ✓ entry is null after read on context 2
  ✓ second write to context 1 succeeded
  ✓ entry size is 4
  ✓ entry is correct
  ✓ entry is null after read on context 2
  ✓ entry is null after delete called
  ✓ memory usage is correctly reported

test emdb
  ✓ database is created
  ✓ entry is null after read
  ✓ write is successful
  ✓ entry size is 4
  ✓ entry is correct

PASSED: 27
FAILED: 0
```

### Using

Currently, you must include the `.o` files that you wish to use in your
linking stage.

There is only one backing store available: `MemoryStorage`.  This is a
reference implementation that is being used to prove out the platform as a
viable engine.

#### Example Program

This is a quick example of interfacing with the database.

```c
#include <stdio.h>

#include "storage/memory.h"
#include "emdb.h"


int main ( ) {
  EMDB *db;
  Entry *entry;

  db = emdb_create_db(&MemoryStorage, 1024, NULL);

  emdb_write(db, (unsigned char *)"key", (unsigned char *)"value", 6);
  entry = emdb_read(db, (unsigned char *)"key");

  printf("found entry: %s\n", (char *)entry->ptr);

  emdb_free_entry(entry);

  emdb_destroy_db(db);

  return 1;
}
```

#### API

##### Creating a Database

Creates a database, as well as the context that it uses.

`EMDB *emdb_create_db(Storage, max_memory, options)`

_Returns:_ `EMDB *` on success, `NULL` on error

```c
EMDB *db = emdb_create_db(&MemoryStorage, 1024, NULL);
```

##### Writing an Entry

Writes a value to the database.  This will overwrite any existing entry.

`unsigned char ret = emdb_write(EMDB *, unsigned char *, unsigned char *, size)`

_Returns:_ `1` on success, `0` on error

```c
emdb_write(db, (unsigned char *) "key", (unsigned char *) "value", 6);
```

##### Reading an Entry

Reads an entry from the database.

`Entry *emdb_read(EMDB *, unsigned char *)`

_Returns:_ `Entry *` on success, `NULL` on error

```c
Entry *entry = emdb_read(db, (unsigned char *) "key");

printf("value: %s\n", (char *) entry->ptr);

emdb_free_entry(entry);
```

##### Deleting an Entry

Deletes and entry from the database.

`unsigned char emdb_delete(EMDB *, unsigned char *)`

_Returns:_ `1` on success, `0` on error

```c
emdb_delete(db, (unsigned char *) "key");
```

##### Closing a Database

Closes a database and frees any resources that are open.

`void emdb_destroy(db)`

```c
emdb_destroy(db);
```

##### Errors

Returns the text of the last known error.

`char *emdb_last_error(EMDB *)`

_Returns:_ Last error thrown

```c
printf("ERROR: %s\n", emdb_last_error(db));
```
