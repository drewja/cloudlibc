// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <cloudabi_syscalls.h>
#include <errno.h>
#include <unistd.h>

ssize_t read(int fildes, void *buf, size_t nbyte) {
  cloudabi_iovec_t iov = {.buf = buf, .buf_len = nbyte};
  size_t bytes_read;
  cloudabi_errno_t error = cloudabi_sys_fd_read(fildes, &iov, 1, &bytes_read);
  if (error != 0) {
    errno = error == ENOTCAPABLE ? EBADF : error;
    return -1;
  }
  return bytes_read;
}
