// util.h
#pragma once

template <typename T> void _log(const char* label, const T& value) {
  Serial.print(label);
  Serial.print(": ");
  Serial.println(value);
}

#define LOG(x) _log(#x, (x))
