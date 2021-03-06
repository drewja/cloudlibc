// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <sys/capsicum.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <errno.h>
#include <fcntl.h>
#include <testing.h>
#include <unistd.h>

TEST(openat, bad) {
  // Invalid file descriptor type.
  int fd = kqueue();
  ASSERT_EQ(-1, openat(fd, "hello", O_WRONLY | O_CREAT));
  ASSERT_EQ(ENOTDIR, errno);
  ASSERT_EQ(0, close(fd));

  // Extension: access is confined to the directory.
  ASSERT_EQ(-1, openat(fd_tmp, "../foo", O_WRONLY | O_CREAT));
  ASSERT_EQ(ENOTCAPABLE, errno);
  ASSERT_EQ(-1, openat(fd_tmp, "/etc/passwd", O_WRONLY | O_CREAT));
  ASSERT_EQ(ENOTCAPABLE, errno);
}

TEST(openat, o_append) {
  // Create a small test file.
  {
    int fd = openat(fd_tmp, "test", O_WRONLY | O_CREAT);
    ASSERT_LE(0, fd);
    ASSERT_EQ(O_WRONLY, fcntl(fd, F_GETFL));
    ASSERT_EQ(5, write(fd, "Hello", 5));
    ASSERT_EQ(0, close(fd));
  }

  // Overwrite the data inside.
  {
    int fd = openat(fd_tmp, "test", O_WRONLY);
    ASSERT_LE(0, fd);
    ASSERT_EQ(O_WRONLY, fcntl(fd, F_GETFL));
    ASSERT_EQ(5, write(fd, "Hello", 5));
    ASSERT_EQ(5, lseek(fd, 0, SEEK_CUR));
    ASSERT_EQ(0, close(fd));
  }

  // Append data to the file.
  {
    int fd = openat(fd_tmp, "test", O_WRONLY | O_APPEND);
    ASSERT_LE(0, fd);
    ASSERT_EQ(O_WRONLY | O_APPEND, fcntl(fd, F_GETFL));
    ASSERT_EQ(5, write(fd, "Hello", 5));
    ASSERT_EQ(10, lseek(fd, 0, SEEK_CUR));
    ASSERT_EQ(0, close(fd));
  }
}

TEST(openat, o_creat) {
  // File does not exist.
  ASSERT_EQ(-1, openat(fd_tmp, "test", O_WRONLY));
  ASSERT_EQ(ENOENT, errno);

  // File has to be created explicitly.
  {
    int fd = openat(fd_tmp, "test", O_WRONLY | O_CREAT);
    ASSERT_LE(0, fd);
    ASSERT_EQ(O_WRONLY, fcntl(fd, F_GETFL));
    ASSERT_EQ(0, close(fd));
  }
}

TEST(openat, o_directory) {
  // Create a file, a fifo and a directory for testing.
  {
    int fd = openat(fd_tmp, "file", O_WRONLY | O_CREAT);
    ASSERT_LE(0, fd);
    ASSERT_EQ(O_WRONLY, fcntl(fd, F_GETFL));
    ASSERT_EQ(0, close(fd));
    ASSERT_EQ(0, mkdirat(fd_tmp, "dir"));
  }

  // Attempt to open them with O_DIRECTORY.
  ASSERT_EQ(-1, openat(fd_tmp, "file", O_RDONLY | O_DIRECTORY));
  ASSERT_EQ(ENOTDIR, errno);
  ASSERT_EQ(-1, openat(fd_tmp, "dir", O_RDWR | O_DIRECTORY));
  ASSERT_EQ(EISDIR, errno);

  // Directory cannot be opened for writing.
  ASSERT_EQ(-1, openat(fd_tmp, "dir", O_RDWR | O_DIRECTORY));
  ASSERT_EQ(EISDIR, errno);

  {
    int fd = openat(fd_tmp, "dir", O_RDONLY | O_DIRECTORY);
    ASSERT_LE(0, fd);
    ASSERT_EQ(O_RDONLY, fcntl(fd, F_GETFL));
    cap_rights_t base, inheriting;
    ASSERT_EQ(0, cap_rights_get_explicit(fd, &base, &inheriting));
    ASSERT_EQ(CAP_CREATE | CAP_EVENT | CAP_FCNTL | CAP_FSTAT | CAP_FSTATAT |
                  CAP_FSYNC | CAP_FUTIMES | CAP_FUTIMESAT | CAP_LINKAT_SOURCE |
                  CAP_LINKAT_TARGET | CAP_LOOKUP | CAP_MKDIRAT |
                  CAP_POSIX_FADVISE | CAP_READDIR | CAP_READLINKAT |
                  CAP_RENAMEAT_SOURCE | CAP_RENAMEAT_TARGET | CAP_SYMLINKAT |
                  CAP_UNLINKAT,
              base.__value);
    ASSERT_EQ(CAP_CREATE | CAP_EVENT | CAP_FCNTL | CAP_FDATASYNC | CAP_FEXECVE |
                  CAP_FSTAT | CAP_FSTATAT | CAP_FSYNC | CAP_FTRUNCATE |
                  CAP_FUTIMES | CAP_FUTIMESAT | CAP_LINKAT_SOURCE |
                  CAP_LINKAT_TARGET | CAP_LOOKUP | CAP_MKDIRAT | CAP_MMAP_RWX |
                  CAP_POSIX_FADVISE | CAP_POSIX_FALLOCATE | CAP_PREAD |
                  CAP_PWRITE | CAP_READ | CAP_READDIR | CAP_READLINKAT |
                  CAP_RENAMEAT_SOURCE | CAP_RENAMEAT_TARGET | CAP_SEEK |
                  CAP_SYMLINKAT | CAP_UNLINKAT | CAP_WRITE,
              inheriting.__value);
    ASSERT_EQ(0, close(fd));
  }
}

TEST(openat, o_excl) {
  // Test file does not exist yet.
  {
    int fd = openat(fd_tmp, "test", O_WRONLY | O_CREAT | O_EXCL);
    ASSERT_LE(0, fd);
    ASSERT_EQ(O_WRONLY, fcntl(fd, F_GETFL));
    ASSERT_EQ(0, close(fd));
  }

  // Second attempt should fail.
  ASSERT_EQ(-1, openat(fd_tmp, "test", O_WRONLY | O_CREAT | O_EXCL));
  ASSERT_EQ(EEXIST, errno);

  // Dropping O_EXCL should make it work again.
  {
    int fd = openat(fd_tmp, "test", O_WRONLY | O_CREAT);
    ASSERT_LE(0, fd);
    ASSERT_EQ(O_WRONLY, fcntl(fd, F_GETFL));
    ASSERT_EQ(0, close(fd));
  }
}

TEST(openat, o_trunc) {
  // Create a small test file.
  {
    int fd = openat(fd_tmp, "test", O_RDWR | O_CREAT);
    ASSERT_LE(0, fd);
    ASSERT_EQ(O_RDWR, fcntl(fd, F_GETFL));
    cap_rights_t base, inheriting;
    ASSERT_EQ(0, cap_rights_get_explicit(fd, &base, &inheriting));
    ASSERT_EQ(CAP_EVENT | CAP_FCNTL | CAP_FDATASYNC | CAP_FEXECVE | CAP_FSTAT |
                  CAP_FSYNC | CAP_FTRUNCATE | CAP_FUTIMES | CAP_MMAP_RWX |
                  CAP_POSIX_FADVISE | CAP_POSIX_FALLOCATE | CAP_PREAD |
                  CAP_PWRITE | CAP_READ | CAP_SEEK | CAP_WRITE,
              base.__value);
    ASSERT_EQ(0, inheriting.__value);
    ASSERT_EQ(5, write(fd, "Hello", 5));
    ASSERT_EQ(0, close(fd));
  }

  // Opening without truncation should preseve file contents.
  {
    int fd = openat(fd_tmp, "test", O_WRONLY | O_CREAT);
    ASSERT_LE(0, fd);
    ASSERT_EQ(O_WRONLY, fcntl(fd, F_GETFL));
    struct stat sb;
    ASSERT_EQ(0, fstat(fd, &sb));
    ASSERT_EQ(5, sb.st_size);
    ASSERT_EQ(0, close(fd));
  }

  // Truncate file contents.
  {
    int fd = openat(fd_tmp, "test", O_WRONLY | O_CREAT | O_TRUNC);
    ASSERT_LE(0, fd);
    ASSERT_EQ(O_WRONLY, fcntl(fd, F_GETFL));
    struct stat sb;
    ASSERT_EQ(0, fstat(fd, &sb));
    ASSERT_EQ(0, sb.st_size);
    ASSERT_EQ(0, close(fd));
  }
}

TEST(openat, o_nofollow) {
  // Create a symbolic link for testing.
  ASSERT_EQ(0, symlinkat("to", fd_tmp, "from"));

  // Depending on whether O_NOFOLLOW is used, the error code should be
  // different.
  ASSERT_EQ(-1, openat(fd_tmp, "from", O_RDONLY | O_NOFOLLOW));
  ASSERT_EQ(ELOOP, errno);
  ASSERT_EQ(-1, openat(fd_tmp, "from", O_RDONLY));
  ASSERT_EQ(ENOENT, errno);
}
