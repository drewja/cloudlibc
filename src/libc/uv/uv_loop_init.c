// Copyright (c) 2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <stdbool.h>
#include <uv.h>

int uv_loop_init(uv_loop_t *loop) {
  uv_update_time(loop);
  loop->__stop = false;
  return 0;
}
