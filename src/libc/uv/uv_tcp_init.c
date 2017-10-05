// Copyright (c) 2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <sys/socket.h>

#include <uv.h>

int uv_tcp_init(uv_loop_t *loop, uv_tcp_t *handle) {
  return uv_tcp_init_ex(loop, handle, AF_UNSPEC);
}
