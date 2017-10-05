// Copyright (c) 2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <uv.h>

#include "uv_impl.h"

int uv_backend_timeout(const uv_loop_t *loop) {
  // Poll timeout calculation as described in step 7 of libuv's design
  // overview, with the exception that the UV_RUN_NOWAIT flag is not
  // taken into account.

  // "If the loop is going to be stopped (uv_stop() was called), the
  // timeout is 0."
  if (loop->__stop)
    return 0;

  // "If there are no active handles or requests, the timeout is 0."
  // TODO(ed): Implement!

  // "If there are any idle handles active, the timeout is 0."
  // TODO(ed): Implement!

  // "If there are any handles pending to be closed, the timeout is 0."
  // TODO(ed): Implement!

  // "If none of the above cases matches, the timeout of the closest
  // timer is taken, or if there are no active timers, infinity."
  const uv_timer_t *timer = __uv_timer_heap_first(&loop->__timers);
  return timer == NULL ? -1 : timer->__timeout;
}
