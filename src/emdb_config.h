#ifndef __EMDB_CONFIG__
#define __EMDB_CONFIG__


#ifdef USE_JSON

// maximum number of tokens per json document
#ifndef JSON_MAX_TOKENS
#define JSON_MAX_TOKENS 16
#endif

#endif /* USE_JSON */

#ifdef USE_QUERY

// maximum depth of keys to search
#define MAX_KEY_DEPTH 4

#endif /* USE_QUERY */

#endif /* __EMDB_CONFIG__ */
