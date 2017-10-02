// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <common/time.h>

#include <sys/select.h>

#include <cloudabi_syscalls.h>
#include <errno.h>

int pselect(int nfds, fd_set *restrict readfds, fd_set *restrict writefds,
            fd_set *restrict errorfds, const struct timespec *restrict timeout,
            ...) {
  // Negative file descriptor upperbound.
  if (nfds < 0) {
    errno = EINVAL;
    return -1;
  }

  // This implementation does not support polling for exceptional
  // conditions, such as out-of-band data on TCP sockets.
  if (errorfds != NULL && errorfds->__nfds > 0) {
    errno = ENOSYS;
    return -1;
  }

  // Replace NULL pointers by the empty set.
  fd_set empty;
  FD_ZERO(&empty);
  if (readfds == NULL)
    readfds = &empty;
  if (writefds == NULL)
    writefds = &empty;

  // Determine the maximum number of events.
  size_t maxevents = readfds->__nfds + writefds->__nfds + 1;
  cloudabi_subscription_t subscriptions[maxevents];
  size_t nevents = 0;

  // Convert the readfds set.
  for (size_t i = 0; i < readfds->__nfds; ++i) {
    int fd = readfds->__fds[i];
    if (fd < nfds) {
      cloudabi_subscription_t *subscription = &subscriptions[nevents++];
      *subscription = (cloudabi_subscription_t){
          .type = CLOUDABI_EVENTTYPE_FD_READABLE,
          .fd_readwritable.fd = fd,
          .fd_readwritable.flags = CLOUDABI_SUBSCRIPTION_FD_READWRITABLE_POLL,
      };
    }
  }

  // Convert the writefds set.
  for (size_t i = 0; i < writefds->__nfds; ++i) {
    int fd = writefds->__fds[i];
    if (fd < nfds) {
      cloudabi_subscription_t *subscription = &subscriptions[nevents++];
      *subscription = (cloudabi_subscription_t){
          .type = CLOUDABI_EVENTTYPE_FD_WRITABLE,
          .fd_readwritable.fd = fd,
          .fd_readwritable.flags = CLOUDABI_SUBSCRIPTION_FD_READWRITABLE_POLL,
      };
    }
  }

  // Create extra event for the timeout.
  if (timeout != NULL) {
    cloudabi_subscription_t *subscription = &subscriptions[nevents++];
    *subscription = (cloudabi_subscription_t){
        .type = CLOUDABI_EVENTTYPE_CLOCK,
        .clock.clock_id = CLOUDABI_CLOCK_REALTIME,
    };
    if (!timespec_to_timestamp_clamp(timeout, &subscription->clock.timeout)) {
      errno = EINVAL;
      return -1;
    }
  }

  // Execute poll().
  cloudabi_event_t events[nevents];
  cloudabi_errno_t error =
      cloudabi_sys_poll(subscriptions, events, nevents, &nevents);
  if (error != 0) {
    errno = error;
    return -1;
  }

  // Test for EBADF.
  for (size_t i = 0; i < nevents; ++i) {
    const cloudabi_event_t *event = &events[i];
    if ((event->type == CLOUDABI_EVENTTYPE_FD_READABLE ||
         event->type == CLOUDABI_EVENTTYPE_FD_WRITABLE) &&
        event->error == CLOUDABI_EBADF) {
      errno = EBADF;
      return -1;
    }
  }

  // Clear and set entries in the result sets.
  FD_ZERO(readfds);
  FD_ZERO(writefds);
  for (size_t i = 0; i < nevents; ++i) {
    const cloudabi_event_t *event = &events[i];
    int fd = event->fd_readwritable.fd;
    if (event->type == CLOUDABI_EVENTTYPE_FD_READABLE) {
      readfds->__fds[readfds->__nfds++] = fd;
    } else if (event->type == CLOUDABI_EVENTTYPE_FD_WRITABLE) {
      writefds->__fds[writefds->__nfds++] = fd;
    }
  }
  return readfds->__nfds + writefds->__nfds;
}
