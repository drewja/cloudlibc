// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <testing.h>
#include <unistd.h>

TEST(dup, file) {
  int fd1 = openat(fd_tmp, "hello", O_CREAT | O_EXCL | O_NONBLOCK | O_WRONLY);
  ASSERT_LE(0, fd1);

  int fd2 = dup(fd1);
  ASSERT_LE(0, fd2);
  ASSERT_NE(fd1, fd2);

  // Both descriptors should point to the same file.
  struct stat sb1, sb2;
  ASSERT_EQ(0, fstat(fd1, &sb1));
  ASSERT_EQ(0, fstat(fd2, &sb2));
  ASSERT_EQ(sb1.st_dev, sb2.st_dev);
  ASSERT_EQ(sb1.st_ino, sb2.st_ino);

  // The O_NONBLOCK flag should be copied along.
  ASSERT_EQ(O_NONBLOCK | O_WRONLY, fcntl(fd1, F_GETFL));
  ASSERT_EQ(O_NONBLOCK | O_WRONLY, fcntl(fd2, F_GETFL));

  ASSERT_EQ(0, close(fd1));
  ASSERT_EQ(0, close(fd2));
}

TEST(dup, bad) {
  ASSERT_EQ(-1, dup(-42));
  ASSERT_EQ(EBADF, errno);
}
