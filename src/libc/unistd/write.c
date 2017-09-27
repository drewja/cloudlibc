// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <sys/uio.h>

#include <unistd.h>

ssize_t write(int fildes, const void *buf, size_t nbyte) {
  struct iovec iov = {.iov_base = (void *)buf, .iov_len = nbyte};
  return writev(fildes, &iov, 1);
}
