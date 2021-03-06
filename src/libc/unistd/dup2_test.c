// Copyright (c) 2015 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <testing.h>
#include <unistd.h>

TEST_SEPARATE_PROCESS(dup2, bad) {
  // Source file descriptor is invalid.
  ASSERT_EQ(-1, dup2(-1, fd_tmp));
  ASSERT_EQ(EBADF, errno);

  // Target file descriptor is invalid.
  ASSERT_EQ(-1, dup2(fd_tmp, -1));
  ASSERT_EQ(EBADF, errno);

  // It is not possible to create new file descriptors using dup2(), for
  // the reason that file descriptor order does not matter in this
  // environment. It is also inherently thread-unsafe.
  int fds[2];
  ASSERT_EQ(0, pipe(fds));
  ASSERT_EQ(0, close(fds[1]));
  ASSERT_EQ(-1, dup2(fds[0], fds[1]));
  ASSERT_EQ(EBADF, errno);
  ASSERT_EQ(0, close(fds[0]));
}

TEST(dup2, good) {
  // Create two files for testing.
  int fd1 = openat(fd_tmp, "file1", O_CREAT | O_EXCL | O_WRONLY);
  ASSERT_LE(0, fd1);
  int fd2 = openat(fd_tmp, "file2", O_CREAT | O_EXCL | O_WRONLY);
  ASSERT_LE(0, fd2);
  ASSERT_NE(fd1, fd2);

  // Both descriptors should point to a different file.
  struct stat sb1, sb2;
  ASSERT_EQ(0, fstat(fd1, &sb1));
  ASSERT_EQ(0, fstat(fd2, &sb2));
  ASSERT_EQ(sb1.st_dev, sb2.st_dev);
  ASSERT_NE(sb1.st_ino, sb2.st_ino);

  ASSERT_EQ(fd2, dup2(fd1, fd2));

  // Both descriptors should now point to the same file.
  ASSERT_EQ(0, fstat(fd1, &sb1));
  ASSERT_EQ(0, fstat(fd2, &sb2));
  ASSERT_EQ(sb1.st_dev, sb2.st_dev);
  ASSERT_EQ(sb1.st_ino, sb2.st_ino);

  ASSERT_EQ(0, close(fd1));
  ASSERT_EQ(0, close(fd2));
}
