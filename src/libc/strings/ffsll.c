// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <strings.h>

#ifndef ffsll
#error "ffsll is supposed to be a macro as well"
#endif

// clang-format off
int (ffsll)(long long i) {
  return ffsll(i);
}
