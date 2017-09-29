// Copyright (c) 2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef COMMON_NONBLOCK_H
#define COMMON_NONBLOCK_H

#include <common/atomic_int_set.h>

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>

static_assert(INT_MAX <= SIZE_MAX,
              "All non-negative integers may be stored in the set");

extern struct atomic_int_set __fd_nonblock_set;

static inline bool __fd_is_nonblock(int fildes) {
  if (fildes < 0)
    return false;
  return atomic_int_set_contains(&__fd_nonblock_set, fildes);
}

static inline bool __fd_set_nonblock(int fildes) {
  if (fildes < 0)
    return false;
  return atomic_int_set_add(&__fd_nonblock_set, fildes);
}

static inline void __fd_clr_nonblock(int fildes) {
  if (fildes >= 0)
    atomic_int_set_remove(&__fd_nonblock_set, fildes);
}

#endif
