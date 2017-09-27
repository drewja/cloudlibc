// Copyright (c) 2015-2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <common/errno.h>

#include <sys/socket.h>

#include <assert.h>
#include <cloudabi_syscalls.h>
#include <errno.h>
#include <stdint.h>

static_assert(MSG_PEEK == CLOUDABI_READ_OPTIONS_PEEK, "Value mismatch");
static_assert(MSG_WAITALL == CLOUDABI_READ_OPTIONS_WAITALL, "Value mismatch");

static_assert(MSG_CTRUNC == CLOUDABI_SOCK_RECV_FDS_TRUNCATED, "Value mismatch");
static_assert(MSG_TRUNC == CLOUDABI_SOCK_RECV_DATA_TRUNCATED, "Value mismatch");

ssize_t recvmsg(int socket, struct msghdr *message, int flags) {
  // Validate flags.
  if ((flags & ~(MSG_PEEK | MSG_WAITALL)) != 0) {
    errno = EOPNOTSUPP;
    return -1;
  }

  // Number of I/O vectors must be positive.
  if (message->msg_iovlen <= 0) {
    errno = EMSGSIZE;
    return -1;
  }

  // Prepare input parameters.
  struct cmsghdr *fds_cmsg = message->msg_control;
  cloudabi_recv_in_t ri = {
      .ri_data = (const cloudabi_iovec_t *)message->msg_iov,
      .ri_data_len = message->msg_iovlen,
      .ri_fds = (cloudabi_fd_t *)CMSG_DATA(fds_cmsg),
      .ri_fds_len =
          message->msg_controllen > CMSG_SPACE(0)
              ? (message->msg_controllen - CMSG_SPACE(0)) / sizeof(int)
              : 0,
      .ri_flags = flags,
  };

  // Perform system call.
  cloudabi_recv_out_t ro;
  cloudabi_errno_t error = cloudabi_sys_sock_recv(socket, &ri, &ro);
  if (error != 0) {
    errno = errno_fixup_socket(socket, error);
    return -1;
  }
  message->msg_flags = ro.ro_flags;

  if (ro.ro_fdslen > 0) {
    // Call yielded received file descriptors. Add SCM_RIGHTS header
    // before the file descriptors and update msg_controllen.
    fds_cmsg->cmsg_len = CMSG_LEN(ro.ro_fdslen * sizeof(int));
    fds_cmsg->cmsg_level = SOL_SOCKET;
    fds_cmsg->cmsg_type = SCM_RIGHTS;
    message->msg_controllen = CMSG_SPACE(ro.ro_fdslen * sizeof(int));
  } else {
    // No ancillary data to return.
    message->msg_controllen = 0;
  }
  return ro.ro_datalen;
}
