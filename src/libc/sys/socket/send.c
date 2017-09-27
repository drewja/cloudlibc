// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <sys/socket.h>

#ifndef send
#error "send is supposed to be a macro as well"
#endif

// clang-format off
ssize_t (send)(int socket, const void *buffer, size_t length, int flags) {
  return send(socket, buffer, length, flags);
}
