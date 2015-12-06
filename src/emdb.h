#ifndef __EMDB_H__
#define __EMDB_H__

/**
 * @file emdb.h
 * @brief Header for EMDB
 */

#ifdef __cplusplus
extern "C" {
#endif

#define EMDB_VERSION "0.1.1"

/**
 * @brief A database entry
 *
 * Entry provides the basis of a database entry.  Memory is allocated
 * for the Entry, and must be freed.  If value is a `String`, the size
 * should include the terminating NULL byte.  Thus, the size of "hello"
 * would be 6, including the NULL termination.
 * @see emdb_free_entry(Entry *)
 */
typedef struct Entry {
  int size; /**< Size of the value stored */
  void *ptr; /**< Pointer to the data that has been stored */
} Entry;

/**
 * @brief An internal representation of a Key
 *
 * Key is an internal reprentation of a Key that can be extended for
 * use in Storage Engines.
 */
typedef struct Key {
  int size; /**< Size of the key stored */
  unsigned char *key; /**< Pointer to the key */
} Key;

/**
 * @brief Statistics Object, used for general statistics.
 *
 * Stats are returned from the Storage Engine in use, and reflect
 * statistics that are tracked internally.
 */
typedef struct Stats {
  unsigned int memory_usage; /**< current memory usage in sizeof(byte) */
  unsigned int entries; /**< current number of entries being tracked */
} Stats;

/**
 * @brief Configuration of a Storage Engine
 *
 * Contains pointers to methods implemented by a Storage Engine.  A Storage
 * Engine must implement all methods.
 */
typedef struct Storage {
  unsigned char (*write)(void *, unsigned char *, unsigned char *, int); /**< write method */
  Entry *(*read)(void *, unsigned char *); /**< read method */
  unsigned char (*delete)(void *, unsigned char *); /**< delete method */
  void (*scan)(void *, void (*)(unsigned char *, Entry *), void (*)(), void (*)(char *)); /**< scan method */
  Stats *(*stats)(void *); /**< stats method */
  char *(*last_error)(void *); /**< last_error method */
  void *(*create_context)(void *); /**< create_context method */
  void (*destroy_context)(void *); /**< destroy_context method */
} Storage;

/**
 * @brief EMDB Object
 *
 * Contains configuration and state.
 */
typedef struct EMDB {
  unsigned short count; /**< count of entries in the database */
  unsigned int memory; /**< total amount of memory in use by the storage engine */
  unsigned int max_memory; /**< an optimistic maximum amount of memory to use */
  int error; /**< last error encountered */
  Storage *store; /**< storage engine in use */
  void *ctx; /**< context for the storage engine */
} EMDB;


/**
 * @brief Creates a database, along with the context for it to be run in.
 *
 * Creates a database, along with the context required for the Storage
 * Engine to operate.
 * @returns EMDB
 * @param store - pointer to the Storage Engine defining operations
 * @param max_memory - maximum amount of memory to optimistically stay under
 * @param cfg - configuration for the Storage Engine, cast as a void *
 */
EMDB *emdb_create_db(Storage *, unsigned int, void *);

/**
 * @brief Closes a database and releases any memory associated
 *
 * Closes a database, releasing any memory or other resources that have
 * been allocated.
 * @param db - database created via emdb_create_db(Storage *, unsigned int, void *)
 */
void emdb_destroy_db(EMDB *);

/**
 * @brief Writes data to Storage Engine.
 *
 * Writes the value passed to the Storage Engine using the key provided.
 * This must include the length of the value to be stored in sizeof(byte),
 * and in the case of a String, should include the NULL byte at the end of
 * the String.
 * @param db - database
 * @param key - key to write, cast as (unsigned char *)
 * @param value - value to write to the Storage Engine
 * @param length - number of bytes _sizeof(byte)_ to write
 * @returns 1 on success, 0 on error
 */
unsigned char emdb_write(EMDB *, unsigned char *, unsigned char *, int);

/**
 * @brief Reads data from the Storage Engine
 *
 * Reads a value from the Storage Engine referenced by the key provided.
 * The Entry returned must be freed after use.
 * @param db - database
 * @param key - key to read, cast as (unsigned char *)
 * @returns Entry *, which must be freed after use using emdb_free_entry(Entry *), or NULL on error or nothing found
 */
Entry *emdb_read(EMDB *, unsigned char *);

/**
 * @brief Deletes data from the Storage Engine
 *
 * Deletes a value from the Storage Engine referenced.
 * @param db - database
 * @param key - key to delete, cast as (unsigned char *)
 * @returns 1 on success, 0 on error
 */
unsigned char emdb_delete(EMDB *, unsigned char *);

/**
 * @brief Returns the last error thrown by either the database or the Storage
 * Engine.
 *
 * If an error exists, returns the String representation of the last error
 * that is thrown by the database or underlying Storage Engine.
 * @param db - database
 * @returns String containing the last known error message, or NULL if none
 */
char *emdb_last_error(EMDB *);

/**
 * @brief Scans all entries in the database, and calls back with key/value
 * pairs, errors, and on end.
 *
 * Executes a scan of all entries currently in the Storage Engine, calling
 * functions for key/value pairs, on end of the scan, and on error.
 * @param db - database
 * @param event_handler - pointer to a function accepting a key, and an Entry
 * @param end_handler - pointer to a function to call at the end of the scan
 * @param error_handler - pointer to a function accepting an error message
 */
void emdb_scan(EMDB *, void (*)(unsigned char *, Entry *), void (*)(), void (*)(char *));

/**
 * @brief Frees memory allocated for an Entry.
 *
 * Frees up all memory that has been allocated for an Entry.
 * @param entry - Entry to free
 */
void emdb_free_entry(Entry *);

/**
 * @brief Copies an Entry.
 *
 * Copies an Entry and allocates any memory needed.  The result must be freed
 * using emdb_free_entry(Entry *).
 * @param entry - Entry to copy
 * @returns Entry containing the new copy
 */
Entry *emdb_copy_entry(Entry *);

#ifdef __cplusplus
}
#endif

#endif /* __EMDB_H__ */
