// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <sys/uio.h>

#include <errno.h>
#include <fcntl.h>
#include <testing.h>
#include <unistd.h>

TEST(pwritev, einval) {
  ASSERT_EQ(-1, pwritev(fd_tmp, NULL, -1, 123));
  ASSERT_EQ(EINVAL, errno);
  ASSERT_EQ(-1, pwritev(fd_tmp, NULL, -1, 123));
  ASSERT_EQ(EINVAL, errno);
}

TEST(pwritev, example) {
  // Prepare example file.
  int fd = openat(fd_tmp, "file", O_CREAT | O_RDWR);
  ASSERT_NE(-1, fd);

  // Perform a pwritev().
  char data1[7] = "Hello, ";
  char data2[5] = "world";
  struct iovec iov[2] = {{.iov_base = data1, .iov_len = sizeof(data1)},
                         {.iov_base = data2, .iov_len = sizeof(data2)}};
  ASSERT_EQ(sizeof(data1) + sizeof(data2), pwritev(fd, iov, 2, 2));

  // Read data back.
  char buf[14];
  ASSERT_EQ(sizeof(buf), read(fd, buf, sizeof(buf)));
  ASSERT_ARREQ("\0\0Hello, world", buf, __arraycount(buf));

  // Close example file.
  ASSERT_EQ(0, close(fd));
}
