// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <sys/types.h>
#include <sys/uio.h>

#include <cloudabi_syscalls.h>
#include <errno.h>

ssize_t pwritev(int fildes, const struct iovec *iov, int iovcnt, off_t offset) {
  if (iovcnt < 0 || offset < 0) {
    errno = EINVAL;
    return -1;
  }
  size_t bytes_written;
  cloudabi_errno_t error = cloudabi_sys_fd_pwrite(
      fildes, (const cloudabi_ciovec_t *)iov, iovcnt, offset, &bytes_written);
  if (error != 0) {
    cloudabi_fdstat_t fds;
    if (error == ENOTCAPABLE && cloudabi_sys_fd_stat_get(fildes, &fds) == 0) {
      // Determine why we got ENOTCAPABLE.
      if ((fds.fs_rights_base & CLOUDABI_RIGHT_FD_WRITE) == 0)
        error = EBADF;
      else
        error = ESPIPE;
    }
    errno = error;
    return -1;
  }
  return bytes_written;
}
