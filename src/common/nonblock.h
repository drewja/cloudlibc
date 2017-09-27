// Copyright (c) 2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef COMMON_NONBLOCK_H
#define COMMON_NONBLOCK_H

#include <stdbool.h>

// TODO(ed): Implement this properly.
static inline bool __fd_is_nonblock(int fildes) {
  return false;
}

#endif
