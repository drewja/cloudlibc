// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <cloudabi_types.h>
#include <pthread.h>
#include <stdatomic.h>

int pthread_spin_init(pthread_spinlock_t *lock, int pshared) {
  atomic_init(&lock->__state, CLOUDABI_LOCK_UNLOCKED);
  lock->__write_recursion = -1;
  lock->__pshared = pshared;
  return 0;
}
