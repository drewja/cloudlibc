// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#define _CLOUDLIBC_UNSAFE_STRING_FUNCTIONS

#include <testing.h>
#include <wchar.h>

TEST(wcscat, example) {
  wchar_t buf[10] = L"\0AAAAAAAAA";
  ASSERT_EQ(buf, wcscat(buf, L""));
  ASSERT_ARREQ(L"\0AAAAAAAAA", buf, __arraycount(buf));
  ASSERT_EQ(buf, wcscat(buf, L"Hello"));
  ASSERT_ARREQ(L"Hello\0AAAA", buf, __arraycount(buf));
  ASSERT_EQ(buf, wcscat(buf, L""));
  ASSERT_ARREQ(L"Hello\0AAAA", buf, __arraycount(buf));
  ASSERT_EQ(buf, wcscat(buf, L"!!!!"));
  ASSERT_ARREQ(L"Hello!!!!\0", buf, __arraycount(buf));
}
