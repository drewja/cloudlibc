// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <string.h>

char *strncpy(char *restrict s1, const char *restrict s2, size_t n) {
  char *begin = s1;
  while (n > 0 && *s2 != '\0') {
    *s1++ = *s2++;
    --n;
  }
  while (n-- > 0)
    *s1++ = '\0';
  return begin;
}
