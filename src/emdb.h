#pragma once

/**
 * @file emdb.h
 * @brief Header for EMDB
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EMDB_VERSION "0.1.1"

// determines whether a storage engine has a specific capability
#define STORAGE_HAS_CAPABILITY(a,b) ((a&b) == b)

#define CAPABILITY_PERMANENT_STORE 1
#define CAPABILITY_INDEXED         2
#define CAPABILITY_OID             4

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
  uint16_t size; /**< Size of the value stored */
  void *ptr; /**< Pointer to the data that has been stored */
} Entry;

/**
 * @brief An internal representation of a Key
 *
 * Key is an internal reprentation of a Key that can be extended for
 * use in Storage Engines.
 */
typedef struct Key {
  uint16_t size; /**< Size of the key stored */
  uint8_t *key; /**< Pointer to the key */
} Key;

/**
 * @brief Statistics Object, used for general statistics.
 *
 * Stats are returned from the Storage Engine in use, and reflect
 * statistics that are tracked internally.
 */
typedef struct Stats {
  uint32_t memory_usage; /**< current memory usage in sizeof(byte) */
  uint16_t entries; /**< current number of entries being tracked */
} Stats;

/**
 * @brief Configuration of a Storage Engine
 *
 * Contains pointers to methods implemented by a Storage Engine.  A Storage
 * Engine must implement all methods.
 */
typedef struct Storage {
  uint8_t (*store_write)(void *, uint8_t *, uint8_t *, uint16_t); /**< write method */
  Entry *(*store_read)(void *, uint8_t *); /**< read method */
  uint8_t (*store_delete)(void *, uint8_t *); /**< delete method */
  void (*scan)(void *, void *, void (*)(void *, uint8_t *, Entry *), void (*)(void *), void (*)(void *, uint8_t *)); /**< scan method */
  Stats *(*stats)(void *); /**< stats method */
  uint8_t *(*last_error)(void *); /**< last_error method */
  void *(*create_context)(void *); /**< create_context method */
  void (*destroy_context)(void *); /**< destroy_context method */
  uint8_t capabilities; /**< storage engine capabilities */
} Storage;

/**
 * @brief EMDB Object
 *
 * Contains configuration and state.
 */
typedef struct EMDB {
  uint16_t count; /**< count of entries in the database */
  uint32_t memory; /**< total amount of memory in use by the storage engine */
  uint32_t max_memory; /**< an optimistic maximum amount of memory to use */
  uint8_t error; /**< last error encountered */
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
EMDB *emdb_create_db(Storage *, uint32_t, void *);

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
uint8_t emdb_write(EMDB *, uint8_t *, uint8_t *, uint16_t);

/**
 * @brief Reads data from the Storage Engine
 *
 * Reads a value from the Storage Engine referenced by the key provided.
 * The Entry returned must be freed after use.
 * @param db - database
 * @param key - key to read, cast as (unsigned char *)
 * @returns Entry *, which must be freed after use using emdb_free_entry(Entry *), or NULL on error or nothing found
 */
Entry *emdb_read(EMDB *, uint8_t *);

/**
 * @brief Deletes data from the Storage Engine
 *
 * Deletes a value from the Storage Engine referenced.
 * @param db - database
 * @param key - key to delete, cast as (unsigned char *)
 * @returns 1 on success, 0 on error
 */
uint8_t emdb_delete(EMDB *, uint8_t *);

/**
 * @brief Returns the last error thrown by either the database or the Storage
 * Engine.
 *
 * If an error exists, returns the String representation of the last error
 * that is thrown by the database or underlying Storage Engine.
 * @param db - database
 * @returns String containing the last known error message, or NULL if none
 */
uint8_t *emdb_last_error(EMDB *);

/**
 * @brief Scans all entries in the database, and calls back with key/value
 * pairs, errors, and on end.
 *
 * Executes a scan of all entries currently in the Storage Engine, calling
 * functions for key/value pairs, on end of the scan, and on error.
 * @param db - database
 * @param context - pointer to a context used for the scan
 * @param event_handler - pointer to a function accepting a context, key, and an Entry
 * @param end_handler - pointer to a function to call at the end of the scan accepting a context
 * @param error_handler - pointer to a function accepting a context, and an error message
 */
void emdb_scan(EMDB *, void *, void (*)(void *, uint8_t *, Entry *), void (*)(void *), void (*)(void *, uint8_t *));

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
