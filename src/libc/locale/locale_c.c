// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <common/locale.h>
#include <common/refcount.h>

#include <locale.h>

struct __locale __locale_c = {
    .refcount = REFCOUNT_INIT(1),

    .collate = &__collate_posix,
    .ctype = &__ctype_us_ascii,
    .messages = &__messages_en_us,
    .monetary = &__monetary_posix,
    .numeric = &__numeric_posix,
    .time = &__time_posix,
    .timezone = &__timezone_utc,
};
