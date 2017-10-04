// Copyright (c) 2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <arpa/inet.h>

#include <errno.h>
#include <uv.h>

int uv_inet_pton(int af, const char *src, void *dst) {
  return inet_pton(af, src, dst) ? 0 : -errno;
}
