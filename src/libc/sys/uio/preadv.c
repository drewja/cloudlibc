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

  // Prepare set of events on which we should wait. If the file
  // descriptor is non-blocking, add an additional zero-value clock to
  // force poll() to return immediately.
  cloudabi_subscription_t subscriptions[2] = {
      {
          .type = CLOUDABI_EVENTTYPE_FD_READ,
          .fd_read =
              {
                  .fd = fildes,
                  .data = (const cloudabi_iovec_t *)iov,
                  .data_len = iovcnt,
                  .whence = CLOUDABI_READWRITE_WHENCE_MANDATORY,
                  .offset = offset,
              },
      },
      {
          .type = CLOUDABI_EVENTTYPE_CLOCK,
          .clock.clock_id = CLOUDABI_CLOCK_MONOTONIC,
      },
  };

  // Wait for the read to complete.
  size_t triggered;
  cloudabi_event_t events[__arraycount(subscriptions)];
  cloudabi_errno_t error = cloudabi_sys_poll(
      subscriptions, events, __fd_is_nonblock(fildes) ? 2 : 1, &triggered);
  if (error != 0) {
    errno = error;
    return -1;
  }

  for (size_t i = 0; i < triggered; ++i) {
    cloudabi_event_t *ev = &events[i];
    if (ev->type == CLOUDABI_EVENTTYPE_FD_READ) {
      if (ev->error != 0) {
        cloudabi_fdstat_t fds;
        if (ev->error == ENOTCAPABLE &&
            cloudabi_sys_fd_stat_get(fildes, &fds) == 0) {
          // Determine why we got ENOTCAPABLE.
          if ((fds.fs_rights_base & CLOUDABI_RIGHT_POLL_FD_READ) == 0)
            ev->error = EBADF;
          else
            ev->error = ESPIPE;
        }
        errno = ev->error;
        return -1;
      }

      // Don't report truncation of read data.
      size_t nbyte = 0;
      for (int j = 0; j < iovcnt; ++j)
        nbyte += iov[j].iov_len;
      return ev->fd_read.datalen < nbyte ? ev->fd_read.datalen : nbyte;
    }
  }

  errno = EAGAIN;
  return -1;
}
