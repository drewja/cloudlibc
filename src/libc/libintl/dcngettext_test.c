// Copyright (c) 2016 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <libintl.h>
#include <locale.h>
#include <testing.h>

TEST(dcngettext, example) {
  ASSERT_STREQ("%d bottles of beer on the wall",
               dcngettext("appname", "%d bottle of beer on the wall",
                          "%d bottles of beer on the wall", 0, LC_MESSAGES));
  ASSERT_STREQ("%d bottle of beer on the wall",
               dcngettext("appname", "%d bottle of beer on the wall",
                          "%d bottles of beer on the wall", 1, LC_MESSAGES));
  ASSERT_STREQ("%d bottles of beer on the wall",
               dcngettext("appname", "%d bottle of beer on the wall",
                          "%d bottles of beer on the wall", 2, LC_MESSAGES));
  ASSERT_STREQ("%d bottles of beer on the wall",
               dcngettext("appname", "%d bottle of beer on the wall",
                          "%d bottles of beer on the wall", 3, LC_MESSAGES));
}
