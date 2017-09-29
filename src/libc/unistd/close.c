// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <common/nonblock.h>

#include <cloudabi_syscalls.h>
#include <errno.h>
#include <unistd.h>

int close(int fildes) {
  __fd_clr_nonblock(fildes);
  cloudabi_errno_t error = cloudabi_sys_fd_close(fildes);
  if (error != 0) {
    errno = error;
    return -1;
  }
  return 0;
}
