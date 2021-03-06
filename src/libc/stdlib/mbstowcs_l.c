// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <stdint.h>
#include <stdlib.h>
#include <wchar.h>

size_t mbstowcs_l(wchar_t *restrict pwcs, const char *restrict s, size_t n,
                  locale_t locale) {
  const char *src = s;
  static const mbstate_t initial_mbstate;
  mbstate_t ps = initial_mbstate;
  return mbsnrtowcs_l(pwcs, &src, SIZE_MAX, n, &ps, locale);
}
