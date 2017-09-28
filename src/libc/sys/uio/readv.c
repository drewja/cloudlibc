// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <common/nonblock.h>

#include <sys/uio.h>

#include <assert.h>
#include <cloudabi_syscalls.h>
#include <errno.h>
#include <stddef.h>

static_assert(offsetof(struct iovec, iov_base) ==
                  offsetof(cloudabi_iovec_t, buf),
              "Offset mismatch");
static_assert(sizeof(((struct iovec *)0)->iov_base) ==
                  sizeof(((cloudabi_iovec_t *)0)->buf),
              "Size mismatch");
static_assert(offsetof(struct iovec, iov_len) ==
                  offsetof(cloudabi_iovec_t, buf_len),
              "Offset mismatch");
static_assert(sizeof(((struct iovec *)0)->iov_len) ==
                  sizeof(((cloudabi_iovec_t *)0)->buf_len),
              "Size mismatch");
static_assert(sizeof(struct iovec) == sizeof(cloudabi_iovec_t),
              "Size mismatch");

ssize_t readv(int fildes, const struct iovec *iov, int iovcnt) {
  if (iovcnt <= 0) {
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
              .whence = CLOUDABI_READWRITE_WHENCE_IGNORED,
          },
  };
  cloudabi_errno_t error = cloudabi_sys_operation_start(&request, 1);
  if (error != 0) {
    // TODO(ed): vv Remove this code once operation_start() works. vv
    if (error == ENOSYS) {
      size_t bytes_read;
      error = cloudabi_sys_fd_read(fildes, (const cloudabi_iovec_t *)iov,
                                   iovcnt, &bytes_read);
      if (error != 0) {
        errno = error == ENOTCAPABLE ? EBADF : error;
        return -1;
      }
      return bytes_read;
    }
    // TODO(ed): ^^ Remove this code once operation_start() works. ^^
    errno = error;
    return -1;
  }
  if (result.error != 0) {
    errno = result.error == ENOTCAPABLE ? EBADF : result.error;
    return -1;
  }

  // Don't report truncation of read data.
  size_t nbyte = 0;
  for (int i = 0; i < iovcnt; ++i)
    nbyte += iov[i].iov_len;
  return result.fd_read.datalen < nbyte ? result.fd_read.datalen : nbyte;
}
