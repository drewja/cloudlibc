// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <sys/uio.h>

#include <errno.h>
#include <fcntl.h>
#include <testing.h>
#include <unistd.h>

TEST(preadv, einval) {
  ASSERT_EQ(-1, preadv(fd_tmp, NULL, -1, 123));
  ASSERT_EQ(EINVAL, errno);
  ASSERT_EQ(-1, preadv(fd_tmp, NULL, 0, 123));
  ASSERT_EQ(EINVAL, errno);
}

TEST(preadv, example) {
  // Prepare example file.
  int fd = openat(fd_tmp, "file", O_CREAT | O_RDWR);
  ASSERT_NE(-1, fd);
  ASSERT_EQ(12, write(fd, "Hello, world", 12));

  // Perform a preadv().
  char data1[5];
  char data2[3];
  struct iovec iov[2] = {{.iov_base = data1, .iov_len = sizeof(data1)},
                         {.iov_base = data2, .iov_len = sizeof(data2)}};
  ASSERT_EQ(sizeof(data1) + sizeof(data2), preadv(fd, iov, 2, 2));
  ASSERT_ARREQ("llo, ", data1, __arraycount(data1));
  ASSERT_ARREQ("wor", data2, __arraycount(data2));

  // Close example file.
  ASSERT_EQ(0, close(fd));
}
