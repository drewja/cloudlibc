// Copyright (c) 2015-2016 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <assert.h>
#include <cloudabi_syscalls.h>
#include <errno.h>
#include <unistd.h>

static_assert(SEEK_CUR == CLOUDABI_SEEK_WHENCE_CUR, "Value mismatch");
static_assert(SEEK_END == CLOUDABI_SEEK_WHENCE_END, "Value mismatch");
static_assert(SEEK_SET == CLOUDABI_SEEK_WHENCE_SET, "Value mismatch");

off_t lseek(int fildes, off_t offset, int whence) {
  cloudabi_filesize_t new_offset;
  cloudabi_errno_t error =
      cloudabi_sys_fd_seek(fildes, offset, whence, &new_offset);
  if (error != 0) {
    errno = error == ENOTCAPABLE ? ESPIPE : error;
    return -1;
  }
  return new_offset;
}
