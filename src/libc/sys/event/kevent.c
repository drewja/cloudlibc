// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <common/time.h>

#include <sys/event.h>
#include <sys/types.h>

#include <assert.h>
#include <cloudabi_syscalls.h>
#include <errno.h>

static_assert(EV_ADD == CLOUDABI_SUBSCRIPTION_ADD, "Value mismatch");
static_assert(EV_CLEAR == CLOUDABI_SUBSCRIPTION_CLEAR, "Value mismatch");
static_assert(EV_DELETE == CLOUDABI_SUBSCRIPTION_DELETE, "Value mismatch");
static_assert(EV_DISABLE == CLOUDABI_SUBSCRIPTION_DISABLE, "Value mismatch");
static_assert(EV_ENABLE == CLOUDABI_SUBSCRIPTION_ENABLE, "Value mismatch");
static_assert(EV_ONESHOT == CLOUDABI_SUBSCRIPTION_ONESHOT, "Value mismatch");

static_assert(EVFILT_TIMER == CLOUDABI_EVENTTYPE_CLOCK, "Value mismatch");
static_assert(EVFILT_READ == CLOUDABI_EVENTTYPE_FD_READABLE, "Value mismatch");
static_assert(EVFILT_WRITE == CLOUDABI_EVENTTYPE_FD_WRITABLE, "Value mismatch");

ssize_t kevent(int fd, const struct kevent *in, size_t nin, struct kevent *out,
               size_t nout, const struct timespec *timeout) {
  // Convert input events.
  cloudabi_subscription_t subscriptions[nin];
  for (size_t i = 0; i < nin; ++i) {
    const struct kevent *ke = &in[i];
    cloudabi_subscription_t *sub = &subscriptions[i];
    *sub = (cloudabi_subscription_t){
        .userdata = (uintptr_t)ke->udata,
        .flags = ke->flags,
        .type = ke->filter,
    };

    // Filter specific data.
    switch (ke->filter) {
      case EVFILT_TIMER:
        sub->clock.identifier = ke->ident;
        sub->clock.clock_id = CLOUDABI_CLOCK_REALTIME;
        sub->clock.timeout = (cloudabi_timestamp_t)ke->data * 1000000;
        break;
      case EVFILT_READ:
      case EVFILT_WRITE:
        sub->fd_readwritable.fd = ke->ident;
        break;
      default:
        errno = EINVAL;
        return -1;
    }
  }

  // Add timeout event.
  cloudabi_subscription_t timosub = {
      .type = CLOUDABI_EVENTTYPE_CLOCK,
      .clock.clock_id = CLOUDABI_CLOCK_REALTIME,
  };
  if (timeout != NULL) {
    if (!timespec_to_timestamp_clamp(timeout, &timosub.clock.timeout)) {
      errno = EINVAL;
      return -1;
    }
  }

  // Invoke poll system call.
  size_t ntriggered;
  cloudabi_event_t events[nout];
  cloudabi_errno_t error =
      cloudabi_sys_poll_fd(fd, subscriptions, nin, events, nout,
                           timeout != NULL ? &timosub : NULL, &ntriggered);
  if (error != 0) {
    errno = error;
    return -1;
  }

  // Convert output events.
  for (size_t i = 0; i < ntriggered; ++i) {
    const cloudabi_event_t *ev = &events[i];
    struct kevent *ke = &out[i];
    *ke = (struct kevent){
        .udata = (void *)ev->userdata,
        .filter = ev->type,
    };
    switch (ev->type) {
      case CLOUDABI_EVENTTYPE_CLOCK:
        ke->ident = ev->clock.identifier;
        break;
      case CLOUDABI_EVENTTYPE_FD_READABLE:
      case CLOUDABI_EVENTTYPE_FD_WRITABLE:
        ke->ident = ev->fd_readwritable.fd;
        break;
    }

    if (ev->error == 0) {
      // Set filter specific properties.
      switch (ev->type) {
        case CLOUDABI_EVENTTYPE_CLOCK:
          // TODO(ed): Propagate the actual count.
          ke->data = 1;
          break;
        case CLOUDABI_EVENTTYPE_FD_READABLE:
        case CLOUDABI_EVENTTYPE_FD_WRITABLE:
          ke->data = ev->fd_readwritable.nbytes;
          if ((ev->fd_readwritable.flags & CLOUDABI_EVENT_FD_READWRITABLE_HANGUP) !=
              0)
            ke->flags |= EV_EOF;
          break;
      }
    } else {
      // An error occurred. Fill the data field with the error code.
      ke->flags |= EV_ERROR;
      ke->data = ev->error;
    }
  }
  return ntriggered;
}
