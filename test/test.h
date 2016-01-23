#ifndef __TEST_H__
#define __TEST_H__

#include <stdint.h>

#ifdef ARDUINO
#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#endif
#else
#include <stdio.h>
#endif

extern uint8_t spec;
extern uint16_t test_passed;
extern uint16_t test_failed;

void _test_fail (const char *, const char *, uint16_t);
void _test_pass (const char *);
void _test_start (const char *);

/* Successfull end of the test case */
#define done() return 0

/* Check single condition */
#define check(cond,message) do { if (!(cond)) { _test_fail(message, __FILE__, __LINE__); test_failed++; } else { _test_pass(message); test_passed++; } } while (0)

/* Test runner */
#define test(func, name) do { _test_start(name); func(); } while(0)

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
uint8_t test_simple_query ( );

#endif

#endif /* __TEST_H__ */
