// Copyright (c) 2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <sys/uio.h>

#include <assert.h>
#include <stddef.h>
#include <uv.h>

static_assert(offsetof(struct iovec, iov_base) == offsetof(uv_buf_t, base),
              "Offset mismatch");
static_assert(sizeof(((struct iovec *)0)->iov_base) ==
                  sizeof(((uv_buf_t *)0)->base),
              "Size mismatch");
static_assert(offsetof(struct iovec, iov_len) == offsetof(uv_buf_t, len),
              "Offset mismatch");
static_assert(sizeof(((struct iovec *)0)->iov_len) ==
                  sizeof(((uv_buf_t *)0)->len),
              "Size mismatch");
static_assert(sizeof(struct iovec) == sizeof(uv_buf_t), "Size mismatch");

uv_buf_t uv_buf_init(char *base, size_t len) {
  return (uv_buf_t){.base = base, .len = len};
}
