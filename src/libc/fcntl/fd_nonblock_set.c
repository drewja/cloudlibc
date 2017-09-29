// Copyright (c) 2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <common/nonblock.h>

#include <stdatomic.h>

static struct atomic_int_bitmask sentinel;
struct atomic_int_set __fd_nonblock_set = {ATOMIC_VAR_INIT(&sentinel)};
