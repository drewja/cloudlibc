// Copyright (c) 2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <cloudabi_syscalls.h>
#include <errno.h>
#include <sigqueue.h>

int sigqueue_create(sigqueue_t *sq) {
  cloudabi_errno_t error =
      cloudabi_sys_fd_create1(CLOUDABI_FILETYPE_POLL, &sq->__poll);
  if (error != 0) {
    errno = error;
    return -1;
  }
  return 0;
}
