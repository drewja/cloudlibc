// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <common/nonblock.h>

#include <sys/types.h>
#include <sys/uio.h>

#include <cloudabi_syscalls.h>
#include <errno.h>

ssize_t preadv(int fildes, const struct iovec *iov, int iovcnt, off_t offset) {
  if (iovcnt <= 0 || offset < 0) {
    errno = EINVAL;
    return -1;
  }

  cloudabi_operation_result_t result;
  cloudabi_operation_request_t request = {
      .type = CLOUDABI_OPERATION_TYPE_FD_READ,
      .execution = __fd_is_nonblock(fildes)
                       ? CLOUDABI_OPERATION_EXECUTION_SYNC_NONBLOCK
                       : CLOUDABI_OPERATION_EXECUTION_SYNC_BLOCK,
      .result = &result,
      .fd_read =
          {
              .fd = fildes,
              .data = (const cloudabi_iovec_t *)iov,
              .data_len = iovcnt,
              .whence = CLOUDABI_READWRITE_WHENCE_MANDATORY,
              .offset = offset,
          },
  };
  cloudabi_errno_t error = cloudabi_sys_operation_start(&request, 1);
  if (error != 0) {
    // TODO(ed): vv Remove this code once operation_start() works. vv
    if (error == ENOSYS) {
      size_t bytes_read;
      error = cloudabi_sys_fd_pread(fildes, (const cloudabi_iovec_t *)iov,
                                    iovcnt, offset, &bytes_read);
      if (error != 0) {
        cloudabi_fdstat_t fds;
        if (error == ENOTCAPABLE &&
            cloudabi_sys_fd_stat_get(fildes, &fds) == 0) {
          // Determine why we got ENOTCAPABLE.
          if ((fds.fs_rights_base & CLOUDABI_RIGHT_FD_READ) == 0)
            error = EBADF;
          else
            error = ESPIPE;
        }
        errno = error;
        return -1;
      }
      return bytes_read;
    }
    // TODO(ed): ^^ Remove this code once operation_start() works. ^^
    errno = error;
    return -1;
  }
  if (result.error != 0) {
    cloudabi_fdstat_t fds;
    if (result.error == ENOTCAPABLE &&
        cloudabi_sys_fd_stat_get(fildes, &fds) == 0) {
      // Determine why we got ENOTCAPABLE.
      if ((fds.fs_rights_base & CLOUDABI_RIGHT_FD_READ) == 0)
        result.error = EBADF;
      else
        result.error = ESPIPE;
    }
    errno = result.error;
    return -1;
  }

  // Don't report truncation of read data.
  size_t nbyte = 0;
  for (int i = 0; i < iovcnt; ++i)
    nbyte += iov[i].iov_len;
  return result.fd_read.datalen < nbyte ? result.fd_read.datalen : nbyte;
}
