#pragma once

#ifdef EMDB_JSON

// maximum number of tokens per json document
#ifndef JSON_MAX_TOKENS

#ifdef TINY
#define JSON_MAX_TOKENS 16
#endif

#ifdef SMALL
#define JSON_MAX_TOKENS 32
#endif

#ifdef MEDIUM
#define JSON_MAX_TOKENS 64
#endif

#ifdef LARGE
#define JSON_MAX_TOKENS 128
#endif

#endif

#endif /* EMDB_JSON */

#ifdef EMDB_QUERY

// maximum depth of keys to search
#ifndef MAX_KEY_DEPTH

#ifdef TINY
#define MAX_KEY_DEPTH 4
#endif

#ifdef SMALL
#define MAX_KEY_DEPTH 16
#endif

#ifdef MEDIUM
#define MAX_KEY_DEPTH 64
#endif

#ifdef LARGE
#define MAX_KEY_DEPTH 128
#endif

#endif

#endif /* EMDB_QUERY */
