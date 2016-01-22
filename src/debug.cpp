#if defined ESP8266 || defined ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#define ARDUINO
#else
#include <iostream>
#endif
#include "debug.h"

void emdb_debug (char *str) {
#ifdef ARDUINO
  Serial.println(str);
#else
  std::cout << str;
#endif
}
