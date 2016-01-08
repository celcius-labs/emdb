#ifndef __TEST_H__
#define __TEST_H__

#include <stdint.h>

extern uint8_t spec;
extern uint16_t test_passed;
extern uint16_t test_failed;

/* Terminate current test with error */
#define fail(message)	{ if (spec) { printf("  𝙭 %s (%s:%d)\n", message, __FILE__, __LINE__); } else { printf("𝙭"); } }

/* Successfull end of the test case */
#define done() return 0

/* Check single condition */
#define check(cond,message) do { if (!(cond)) { fail(message); test_failed++; } else { if (spec) { printf("  ✓ %s\n", message); } else { printf("."); } test_passed++; } } while (0)

/* Test runner */
#define test(func, name) do { if (spec) { printf("\n%s\n", name); } func(); } while(0)

uint8_t test_memory ( );
uint8_t test_emdb ( );
uint8_t test_context_isolation ( );

#ifdef EMDB_QUERY

uint8_t test_compare_string ( );
uint8_t test_compare_int ( );
uint8_t test_compare_float ( );
uint8_t test_and_list ( );
uint8_t test_or_list ( );

#endif /* EMDB_QUERY */

#ifdef EMDB_JSON

uint8_t test_json_utils ( );
uint8_t test_int_from_json ( );
uint8_t test_float_from_json ( );
uint8_t test_string_from_json ( );

#endif

#endif /* __TEST_H__ */
