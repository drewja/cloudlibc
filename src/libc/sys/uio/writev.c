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
                  offsetof(cloudabi_ciovec_t, buf),
              "Offset mismatch");
static_assert(sizeof(((struct iovec *)0)->iov_base) ==
                  sizeof(((cloudabi_ciovec_t *)0)->buf),
              "Size mismatch");
static_assert(offsetof(struct iovec, iov_len) ==
                  offsetof(cloudabi_ciovec_t, buf_len),
              "Offset mismatch");
static_assert(sizeof(((struct iovec *)0)->iov_len) ==
                  sizeof(((cloudabi_ciovec_t *)0)->buf_len),
              "Size mismatch");
static_assert(sizeof(struct iovec) == sizeof(cloudabi_ciovec_t),
              "Size mismatch");

ssize_t writev(int fildes, const struct iovec *iov, int iovcnt) {
  if (iovcnt <= 0) {
    errno = EINVAL;
    return -1;
  }

  cloudabi_operation_result_t result;
  cloudabi_operation_request_t request = {
      .type = CLOUDABI_OPERATION_TYPE_FD_WRITE,
      .execution = __fd_is_nonblock(fildes)
                       ? CLOUDABI_OPERATION_EXECUTION_SYNC_NONBLOCK
                       : CLOUDABI_OPERATION_EXECUTION_SYNC_BLOCK,
      .result = &result,
      .fd_write =
          {
              .fd = fildes,
              .data = (const cloudabi_ciovec_t *)iov,
              .data_len = iovcnt,
              .whence = CLOUDABI_READWRITE_WHENCE_IGNORED,
          },
  };
  cloudabi_errno_t error = cloudabi_sys_operation_start(&request, 1);
  if (error != 0) {
    // TODO(ed): vv Remove this code once operation_start() works. vv
    if (error == ENOSYS) {
      size_t bytes_written;
      error = cloudabi_sys_fd_write(fildes, (const cloudabi_ciovec_t *)iov,
                                    iovcnt, &bytes_written);
      if (error != 0) {
        errno = error == ENOTCAPABLE ? EBADF : error;
        return -1;
      }
      return bytes_written;
    }
    // TODO(ed): ^^ Remove this code once operation_start() works. ^^
    errno = error;
    return -1;
  }
  if (result.error != 0) {
    errno = result.error == ENOTCAPABLE ? EBADF : result.error;
    return -1;
  }
  return result.fd_write.datalen;
}
