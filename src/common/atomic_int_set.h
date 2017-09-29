// Copyright (c) 2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef COMMON_ATOMIC_INT_SET_H
#define COMMON_ATOMIC_INT_SET_H

#include <assert.h>
#include <limits.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

// Atomic integer sets.
//
// For storing properties like per-fd flags, we want to have a set,
// capable of holding integer values. This set also has to be
// thread-safe, but also save for forking. In other words, we want it to
// be lockless.
//
// The functions in this file implement an atomic bitmask. Whenever the
// bitmask is too small to store a value, an additional bitmask is
// allocated to store the next range of numbers. All of these bitmasks
// are stored in a linked list that is updated atomically.
//
// As the size of allocated bitmasks grows exponentially, this data
// structure guaranteed O(log n) lookup time.
//
// TODO(ed): Deal with arbitrarily high numbers?

static_assert(ATOMIC_LONG_LOCK_FREE == 2,
              "long is required to be lock-free for this data structure");

struct atomic_int_bitmask {
  struct atomic_int_bitmask *previous;  // Previous, smaller bitmask.
  size_t end;                           // First value outside the range.
  atomic_ulong entries[];               // Atomic bitmask.
};

struct atomic_int_set {
  _Atomic(struct atomic_int_bitmask *) last;  // Last allocated bitmask.
};

static bool atomic_int_set_contains(struct atomic_int_set *set, size_t value) {
  // The number must be inside the bounds of the set.
  struct atomic_int_bitmask *bitmask =
      atomic_load_explicit(&set->last, memory_order_acquire);
  if (value >= bitmask->end)
    return false;

  // Find the bitmask that could contain the value.
  while (value < bitmask->previous->end)
    bitmask = bitmask->previous;

  // Check whether the bit corresponding with the value is set.
  value -= bitmask->previous->end;
  return (atomic_load(&bitmask->entries[value / LONG_BIT]) &
          (1UL << (value % LONG_BIT))) != 0;
}

static bool atomic_int_set_add(struct atomic_int_set *set, size_t value) {
  // Prepend an additional bitmask in case the existing set is too small
  // to contain the value.
  struct atomic_int_bitmask *bitmask =
      atomic_load_explicit(&set->last, memory_order_acquire);
  while (value >= bitmask->end) {
    // Let the new bitmask be a power of two in size. Require that it at
    // least doubles the set size, so that the number of bitmasks can at
    // most be logarithmic.
    size_t new_size = 128;
    size_t new_bits, new_bits_end;
    for (;;) {
      new_bits = (new_size - offsetof(struct atomic_int_bitmask, entries)) /
                 sizeof(atomic_ulong) * LONG_BIT;
      new_bits_end = bitmask->end + new_bits;
      if (new_bits_end > value && new_bits >= bitmask->end)
        break;
      new_size *= 2;
    }

    // Allocate and initialize the new bitmask.
    struct atomic_int_bitmask *new_bitmask = malloc(new_size);
    if (new_bitmask == NULL)
      return false;
    new_bitmask->previous = bitmask;
    new_bitmask->end = new_bits_end;
    for (size_t i = 0; i < __howmany(new_bits, LONG_BIT); ++i)
      atomic_init(&new_bitmask->entries[i], 0);

    // Append it to the list of bitmasks.
    if (atomic_compare_exchange_weak_explicit(&set->last, &bitmask, new_bitmask,
                                              memory_order_acq_rel,
                                              memory_order_acquire)) {
      bitmask = new_bitmask;
      break;
    }
    free(new_bitmask);
  }

  // Find the bitmask that could contain the value.
  while (value < bitmask->previous->end)
    bitmask = bitmask->previous;

  // Set the bit corresponding with the value.
  value -= bitmask->previous->end;
  atomic_fetch_or(&bitmask->entries[value / LONG_BIT],
                  1UL << (value % LONG_BIT));
  return true;
}

static void atomic_int_set_remove(struct atomic_int_set *set, size_t value) {
  // The number must be inside the bounds of the set.
  struct atomic_int_bitmask *bitmask =
      atomic_load_explicit(&set->last, memory_order_acquire);
  if (value < bitmask->end) {
    // Find the bitmask that could contain the value.
    while (value < bitmask->previous->end)
      bitmask = bitmask->previous;

    // Clear the bit corresponding with the value.
    value -= bitmask->previous->end;
    atomic_fetch_and(&bitmask->entries[value / LONG_BIT],
                     ~(1UL << (value % LONG_BIT)));
  }
}

#endif
