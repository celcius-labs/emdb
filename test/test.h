#ifndef __TEST_H__
#define __TEST_H__

extern int spec;
extern int test_passed;
extern int test_failed;

/* Terminate current test with error */
#define fail(message)	{ if (spec) { printf("  𝙭 %s (%d)\n", message, __LINE__); } else { printf("𝙭"); } }

/* Successfull end of the test case */
#define done() return 0

/* Check single condition */
#define check(cond,message) do { if (!(cond)) { fail(message); test_failed++; } else { if (spec) { printf("  ✓ %s\n", message); } else { printf("."); } test_passed++; } } while (0)

/* Test runner */
static void test (int (*func)(void), const char *name) {
  if (spec) {
    printf("\n%s\n", name);
  }
  func();
}

int test_memory ( );
int test_emdb ( );
int test_context_isolation ( );

#endif /* __TEST_H__ */
