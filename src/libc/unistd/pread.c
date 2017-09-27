// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <sys/uio.h>

#include <unistd.h>

ssize_t pread(int fildes, void *buf, size_t nbyte, off_t offset) {
  struct iovec iov = {.iov_base = buf, .iov_len = nbyte};
  return preadv(fildes, &iov, 1, offset);
}
