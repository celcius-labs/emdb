#include <stdio.h>
#include <string.h>

#include "query.h"


#include "test.h"


int test_compare_string ( ) {
  // equals
  char *text1 = "hello";
  char *text2 = "foo";

  check(compare_string(text1, text1, equals) == 1, "same text is considered equal");
  check(compare_string(text1, text2, equals) == 0, "different text is not considered equal");
  check(compare_string(text1, text1, gt) == 0, "equal text is not considered gt");
  check(compare_string(text1, text2, gt) == 1, "gt text is considered gt");
  check(compare_string(text2, text1, gt) == 0, "lt text is not considered gt");
  check(compare_string(text1, text1, lt) == 0, "equal text is not considered lt");
  check(compare_string(text2, text1, lt) == 1, "lt text is considered lt");
  check(compare_string(text1, text2, lt) == 0, "gt text is not considered lt");
  check(compare_string(text1, text1, gte) == 1, "same text is considered gte");
  check(compare_string(text1, text2, gte) == 1, "gt text is considered gte");
  check(compare_string(text1, text1, lte) == 1, "same text is considered lte");
  check(compare_string(text2, text1, lte) == 1, "lt text is considered lte");

  done();
}

int test_compare_int ( ) {
  int i1 = 10;
  int i2 = 20;

  check(compare_int(i1, i1, equals) == 1, "same integer is considered equal");
  check(compare_int(i1, i2, equals) == 0, "different integer is not considered equal");
  check(compare_int(i1, i1, gt) == 0, "equal integer is not considered gt");
  check(compare_int(i2, i1, gt) == 1, "gt integer is considered gt");
  check(compare_int(i1, i2, gt) == 0, "lt integer is not considered gt");
  check(compare_int(i1, i1, lt) == 0, "equal integer is not considered lt");
  check(compare_int(i1, i2, lt) == 1, "lt integer is considered lt");
  check(compare_int(i2, i1, lt) == 0, "gt integer is not considered lt");
  check(compare_int(i2, i1, gte) == 1, "gt integer is considered gte");
  check(compare_int(i1, i2, gte) == 0, "lt integer is not considered gte");
  check(compare_int(i1, i1, gte) == 1, "equal integer is considered gte");
  check(compare_int(i1, i1, lte) == 1, "equal integer is considered lte");
  check(compare_int(i1, i2, lte) == 1, "lt integer is considered lte");
  check(compare_int(i2, i1, lte) == 0, "gt integer is not considered lte");

  done();
}

int test_compare_float ( ) {
  float f1 = 1.0;
  float f2 = 2.0;

  check(compare_float(f1, f1, equals) == 1, "same float is considered equal");
  check(compare_float(f1, f2, equals) == 0, "different float is not considered equal");
  check(compare_float(f1, f1, gt) == 0, "equal float is not considered gt");
  check(compare_float(f2, f1, gt) == 1, "gt float is considered gt");
  check(compare_float(f1, f2, gt) == 0, "lt float is not considered gt");
  check(compare_float(f1, f1, lt) == 0, "equal float is not considered lt");
  check(compare_float(f1, f2, lt) == 1, "lt float is considered lt");
  check(compare_float(f2, f1, lt) == 0, "gt float is not considered lt");
  check(compare_float(f2, f1, gte) == 1, "gt float is considered gte");
  check(compare_float(f1, f2, gte) == 0, "lt float is not considered gte");
  check(compare_float(f1, f1, gte) == 1, "equal float is considered gte");
  check(compare_float(f1, f1, lte) == 1, "equal float is considered lte");
  check(compare_float(f1, f2, lte) == 1, "lt float is considered lte");
  check(compare_float(f2, f1, lte) == 0, "gt float is not considered lte");

  done();
}
