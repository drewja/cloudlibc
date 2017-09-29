// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <common/nonblock.h>

#include <sys/ioctl.h>

#include <cloudabi_syscalls.h>
#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>

int ioctl(int fildes, int request, ...) {
  switch (request) {
    case FIONREAD: {
      // Poll the file descriptor to determine how many bytes can be read.
      cloudabi_subscription_t subscriptions[2] = {
          {
              .type = CLOUDABI_EVENTTYPE_FD_READ,
              .fd_readwrite.fd = fildes,
          },
          {
              .type = CLOUDABI_EVENTTYPE_CLOCK,
              .clock.clock_id = CLOUDABI_CLOCK_MONOTONIC,
          },
      };
      cloudabi_event_t events[__arraycount(subscriptions)];
      size_t nevents;
      cloudabi_errno_t error = cloudabi_sys_poll(
          subscriptions, events, __arraycount(subscriptions), &nevents);
      if (error != 0) {
        errno = error;
        return -1;
      }

      // Location where result should be written.
      va_list ap;
      va_start(ap, request);
      int *result = va_arg(ap, int *);
      va_end(ap);

      // Extract number of bytes for reading from poll results.
      for (size_t i = 0; i < nevents; ++i) {
        cloudabi_event_t *event = &events[i];
        if (event->error != 0) {
          errno = event->error;
          return -1;
        }
        if (event->type == CLOUDABI_EVENTTYPE_FD_READ) {
          *result = event->fd_readwrite.nbytes;
          return 0;
        }
      }

      // No data available for reading.
      *result = 0;
      return 0;
    }
    case FIONBIO: {
      va_list ap;
      va_start(ap, request);
      bool nonblock = *va_arg(ap, const int *) != 0;
      va_end(ap);

      // TODO(ed): vv Remove this code once operation_start() works. vv
      // Obtain the current file descriptor flags.
      cloudabi_fdstat_t fds;
      cloudabi_errno_t error = cloudabi_sys_fd_stat_get(fildes, &fds);
      if (error != 0) {
        errno = error;
        return -1;
      }

      // Toggle the non-blocking flag based on the argument.
      if (nonblock)
        fds.fs_flags |= CLOUDABI_FDFLAG_NONBLOCK;
      else
        fds.fs_flags &= ~CLOUDABI_FDFLAG_NONBLOCK;

      // Update the file descriptor flags.
      error = cloudabi_sys_fd_stat_put(fildes, &fds, CLOUDABI_FDSTAT_FLAGS);
      if (error != 0) {
        errno = error;
        return -1;
      }
      // TODO(ed): ^^ Remove this code once operation_start() works. ^^

      if (nonblock) {
        if (!__fd_set_nonblock(fildes)) {
          errno = ENOMEM;
          return -1;
        }
      } else {
        __fd_clr_nonblock(fildes);
      }
      return 0;
    }
    default:
      // Invalid request.
      errno = EINVAL;
      return -1;
  }
}
