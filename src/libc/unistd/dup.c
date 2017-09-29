// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <common/nonblock.h>

#include <cloudabi_syscalls.h>
#include <errno.h>
#include <unistd.h>

int dup(int fildes) {
  cloudabi_fd_t newfd;
  cloudabi_errno_t error = cloudabi_sys_fd_dup(fildes, &newfd);
  if (error != 0) {
    errno = error;
    return -1;
  }

  // Copy over the O_NONBLOCK flag.
  if (__fd_is_nonblock(fildes)) {
    if (!__fd_set_nonblock(newfd)) {
      cloudabi_sys_fd_close(newfd);
      errno = ENOMEM;
      return -1;
    }
  } else {
    __fd_clr_nonblock(newfd);
  }
  return newfd;
}
