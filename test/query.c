#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

  check(compare_string_between("goo", text2, text1) == 1, "between returns true when between");
  check(compare_string_between(text2, text2, text1) == 1, "between returns true when gte");
  check(compare_string_between(text1, text2, text1) == 1, "between returns true when lte");
  check(compare_string_between("ho", text2, text1) == 0, "between returns false when gt");
  check(compare_string_between("fa", text2, text1) == 0, "between returns false when lt");

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

  check(compare_int_between(15, i2, i1) == 1, "between returns true when between");
  check(compare_int_between(20, i2, i1) == 1, "between returns true when gte");
  check(compare_int_between(10, i2, i1) == 1, "between returns true when lte");
  check(compare_int_between(21, i2, i1) == 0, "between returns false when gt");
  check(compare_int_between(9, i2, i1) == 0, "between returns false when lt");

  check(compare_int_between(15, i1, i2) == 1, "between returns true when between reversed");
  check(compare_int_between(20, i1, i2) == 1, "between returns true when gte reversed");
  check(compare_int_between(10, i1, i2) == 1, "between returns true when lte reversed");
  check(compare_int_between(21, i1, i2) == 0, "between returns false when gt reversed");
  check(compare_int_between(9, i1, i2) == 0, "between returns false when lt reversed");

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

  check(compare_float_between(1.5, f2, f1) == 1, "between returns true when between");
  check(compare_float_between(2.0, f2, f1) == 1, "between returns true when gte");
  check(compare_float_between(1.0, f2, f1) == 1, "between returns true when lte");
  check(compare_float_between(2.1, f2, f1) == 0, "between returns false when gt");
  check(compare_float_between(0.9, f2, f1) == 0, "between returns false when lt");

  check(compare_float_between(1.5, f1, f2) == 1, "between returns true when between reversed");
  check(compare_float_between(2.0, f1, f2) == 1, "between returns true when gte reversed");
  check(compare_float_between(1.0, f1, f2) == 1, "between returns true when lte reversed");
  check(compare_float_between(2.1, f1, f2) == 0, "between returns false when gt reversed");
  check(compare_float_between(0.9, f1, f2) == 0, "between returns false when lt reversed");

  done();
}

int test_and_list ( ) {
  char *list1[6] = {
    "one",
    "two",
    "three",
    "four",
    "five",
    "six"
  };

  char *list2[4] = {
    "one",
    "two",
    "three"
  };

  int len = 0, i;
  char **results;

  results = and_list(list1, list2, 6, 3);

  for (i = 0; i < 5; i++) {
    if (results[i] == NULL) {
      len = i;
      break;
    }
  }

  check(len == 3, "and results have a length of 3");
  check((strcmp(results[0], "one") == 0), "first result is correct");
  check((strcmp(results[1], "two") == 0), "second result is correct");
  check((strcmp(results[2], "three") == 0), "third result is correct");

  free(results);

  done();
}

int test_or_list ( ) {
  char *list1[6] = {
    "one",
    "two",
    "three",
    "four",
    "five",
    "six"
  };

  char *list2[4] = {
    "one",
    "two",
    "three"
  };

  char *list3[4] = {
    "seven",
    "eight",
    "nine"
  };

  int len = 0, i;
  char **results;

  results = or_list(list1, list2, 6, 3);

  for (i = 0; i < 10; i++) {
    if (results[i] == NULL) {
      len = i;
      break;
    }
  }

  check(len == 6, "and results have a length of 6");
  check((strcmp(results[0], "one") == 0), "first result is correct");
  check((strcmp(results[1], "two") == 0), "second result is correct");
  check((strcmp(results[2], "three") == 0), "third result is correct");
  check((strcmp(results[3], "four") == 0), "fourth result is correct");
  check((strcmp(results[4], "five") == 0), "fifth result is correct");
  check((strcmp(results[5], "six") == 0), "sixth result is correct");

  free(results);

  results = or_list(list1, list3, 6, 3);

  for (i = 0; i < 10; i++) {
    if (results[i] == NULL) {
      len = i;
      break;
    }
  }

  check(len == 9, "and results have a length of 9");
  check((strcmp(results[0], "one") == 0), "first result is correct");
  check((strcmp(results[1], "two") == 0), "second result is correct");
  check((strcmp(results[2], "three") == 0), "third result is correct");
  check((strcmp(results[3], "four") == 0), "fourth result is correct");
  check((strcmp(results[4], "five") == 0), "fifth result is correct");
  check((strcmp(results[5], "six") == 0), "sixth result is correct");
  check((strcmp(results[6], "seven") == 0), "seventh result is correct");
  check((strcmp(results[7], "eight") == 0), "eighth result is correct");
  check((strcmp(results[8], "nine") == 0), "nineth result is correct");

  free(results);

  done();
}
