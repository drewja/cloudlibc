// Copyright (c) 2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <pthread.h>
#include <stdlib.h>
#include <uv.h>

void uv_cond_destroy(uv_cond_t *cond) {
  if (pthread_cond_destroy(cond) != 0)
    abort();
}
