// Copyright (c) 2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <uv.h>

int uv_loop_close(uv_loop_t *loop) {
  // TODO(ed): Free memory and return UV_EBUSY when needed.
  return 0;
}
