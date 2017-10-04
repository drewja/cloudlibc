// Copyright (c) 2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <uv.h>

int uv_translate_sys_error(int sys_errno) {
  return sys_errno < 0 ? sys_errno : -sys_errno;
}
