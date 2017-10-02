// Copyright (c) 2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <cloudabi_syscalls.h>
#include <sigqueue.h>

void sigqueue_delete(sigqueue_t sq) {
  cloudabi_sys_fd_close(sq.__poll);
}
