// Copyright (c) 2017 Nuxi, https://nuxi.nl/
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

// <uv.h> - event loops

#ifndef _UV_H_
#define _UV_H_

#include <_/types.h>

#ifndef _FILE_DECLARED
typedef struct _FILE FILE;
#define _FILE_DECLARED
#endif
#ifndef _SIZE_T_DECLARED
typedef __size_t size_t;
#define _SIZE_T_DECLARED
#endif
#ifndef _SSIZE_T_DECLARED
typedef __ssize_t ssize_t;
#define _SSIZE_T_DECLARED
#endif
#ifndef _UINT64_T_DECLARED
typedef __uint64_t uint64_t;
#define _UINT64_T_DECLARED
#endif

struct sockaddr_in;
struct sockaddr_in6;

//
// Miscellaneous utilities.
//

typedef struct {
  char *base;
  size_t len;
} uv_buf_t;

typedef int uv_file;
typedef int uv_os_fd_t;
typedef int uv_os_sock_t;

__BEGIN_DECLS
uv_buf_t uv_buf_init(char *, size_t);
int uv_os_getenv(const char *, char *, size_t *);
uint64_t uv_hrtime(void);
int uv_inet_ntop(int, const void *, char *, size_t);
int uv_inet_pton(int, const char *, void *);
int uv_ip4_addr(const char *, int, struct sockaddr_in *);
int uv_ip6_addr(const char *, int, struct sockaddr_in6 *);
int uv_ip4_name(const struct sockaddr_in *, char *, size_t);
int uv_ip6_name(const struct sockaddr_in6 *, char *, size_t);
#if 0  // TODO(ed): Add these.
uv_handle_type uv_guess_handle(uv_file);
void uv_print_active_handles(uv_loop_t*, FILE*);
void uv_print_all_handles(uv_loop_t*, FILE*);
#endif
__END_DECLS

//
// Error handling.
//

typedef enum {
  // C/POSIX error codes. Map these to [-1000, 0).
  UV_E2BIG = -1,
  UV_EACCES = -2,
  UV_EADDRINUSE = -3,
  UV_EADDRNOTAVAIL = -4,
  UV_EAFNOSUPPORT = -5,
  UV_EAGAIN = -6,
  UV_EALREADY = -7,
  UV_EBADF = -8,
  UV_EBUSY = -10,
  UV_ECANCELED = -11,
  UV_ECONNABORTED = -13,
  UV_ECONNREFUSED = -14,
  UV_ECONNRESET = -15,
  UV_EDESTADDRREQ = -17,
  UV_EEXIST = -20,
  UV_EFAULT = -21,
  UV_EFBIG = -22,
  UV_EHOSTUNREACH = -23,
  UV_EINTR = -27,
  UV_EINVAL = -28,
  UV_EIO = -29,
  UV_EISCONN = -30,
  UV_EISDIR = -31,
  UV_ELOOP = -32,
  UV_EMFILE = -33,
  UV_EMLINK = -34,
  UV_EMSGSIZE = -35,
  UV_ENAMETOOLONG = -37,
  UV_ENETDOWN = -38,
  UV_ENETUNREACH = -40,
  UV_ENFILE = -41,
  UV_ENOBUFS = -42,
  UV_ENODEV = -43,
  UV_ENOENT = -44,
  UV_ENOMEM = -48,
  UV_ENOPROTOOPT = -50,
  UV_ENOSPC = -51,
  UV_ENOSYS = -52,
  UV_ENOTCONN = -53,
  UV_ENOTDIR = -54,
  UV_ENOTEMPTY = -55,
  UV_ENOTSOCK = -57,
  UV_ENOTSUP = -58,
  UV_ENXIO = -60,
  UV_EPERM = -63,
  UV_EPIPE = -64,
  UV_EPROTO = -65,
  UV_EPROTONOSUPPORT = -66,
  UV_EPROTOTYPE = -67,
  UV_ERANGE = -68,
  UV_EROFS = -69,
  UV_ESPIPE = -70,
  UV_ESRCH = -71,
  UV_ETIMEDOUT = -73,
  UV_ETXTBSY = -74,
  UV_EXDEV = -75,

  // getaddrinfo() error codes. Map these to [-2000,-1000).
  UV_EAI_AGAIN = -1001,
  UV_EAI_BADFLAGS = -1002,
  UV_EAI_FAIL = -1003,
  UV_EAI_FAMILY = -1004,
  UV_EAI_MEMORY = -1005,
  UV_EAI_NONAME = -1006,
  UV_EAI_OVERFLOW = -1010,
  UV_EAI_SERVICE = -1007,
  UV_EAI_SOCKTYPE = -1008,

  // libuv specific error codes. Map these to [-3000,-2000).
  UV_EAI_ADDRFAMILY = -2001,
  UV_EAI_BADHINTS = -2002,
  UV_EAI_CANCELED = -2003,
  UV_EAI_NODATA = -2004,
  UV_EAI_PROTOCOL = -2005,
  UV_ECHARSET = -2006,
  UV_ENONET = -2007,
  UV_EOF = -2008,
  UV_ESHUTDOWN = -2009,
  UV_UNKNOWN = -2010,
} uv_errno_t;

__BEGIN_DECLS
const char *uv_err_name(int);
const char *uv_strerror(int);
int uv_translate_sys_error(int);
__END_DECLS

//
// Threading and synchronization utilities.
//

typedef __pthread_barrier_t uv_barrier_t;
typedef __pthread_cond_t uv_cond_t;
typedef __pthread_key_t uv_key_t;
typedef __pthread_lock_t uv_mutex_t;
typedef __pthread_once_t uv_once_t;
typedef __pthread_lock_t uv_rwlock_t;
typedef __sem_t uv_sem_t;
typedef void (*uv_thread_cb)(void *);
typedef __pthread_t uv_thread_t;

#define UV_ONCE_INIT \
  { _UINT32_C(0x80000000) }

__BEGIN_DECLS
void uv_barrier_destroy(uv_barrier_t *);
int uv_barrier_init(uv_barrier_t *barrier, unsigned int);
int uv_barrier_wait(uv_barrier_t *);
void uv_cond_broadcast(uv_cond_t *);
void uv_cond_destroy(uv_cond_t *);
int uv_cond_init(uv_cond_t *);
void uv_cond_signal(uv_cond_t *);
int uv_cond_timedwait(uv_cond_t *, uv_mutex_t *__mutex, uint64_t)
    __requires_exclusive(*__mutex);
void uv_cond_wait(uv_cond_t *, uv_mutex_t *__mutex)
    __requires_exclusive(*__mutex);
int uv_key_create(uv_key_t *);
void uv_key_delete(uv_key_t *);
void *uv_key_get(uv_key_t *);
void uv_key_set(uv_key_t *, void *);
void uv_mutex_destroy(uv_mutex_t *__handle) __requires_unlocked(*__handle);
int uv_mutex_init(uv_mutex_t *__handle) __requires_unlocked(*__handle);
void uv_mutex_lock(uv_mutex_t *__handle) __locks_exclusive(*__handle);
int uv_mutex_trylock(uv_mutex_t *__handle) __trylocks_exclusive(0, *__handle);
void uv_mutex_unlock(uv_mutex_t *__handle) __unlocks(*__handle);
void uv_once(uv_once_t *, void (*)(void));
void uv_rwlock_destroy(uv_rwlock_t *__rwlock) __requires_unlocked(*__rwlock);
int uv_rwlock_init(uv_rwlock_t *__rwlock) __requires_unlocked(*__rwlock);
void uv_rwlock_rdlock(uv_rwlock_t *__rwlock) __locks_shared(*__rwlock);
void uv_rwlock_rdunlock(uv_rwlock_t *__rwlock) __unlocks(*__rwlock);
int uv_rwlock_tryrdlock(uv_rwlock_t *__rwlock) __trylocks_shared(0, *__rwlock);
int uv_rwlock_trywrlock(uv_rwlock_t *__rwlock)
    __trylocks_exclusive(0, *__rwlock);
void uv_rwlock_wrlock(uv_rwlock_t *__rwlock) __locks_exclusive(*__rwlock);
void uv_rwlock_wrunlock(uv_rwlock_t *__rwlock) __unlocks(*__rwlock);
void uv_sem_destroy(uv_sem_t *);
int uv_sem_init(uv_sem_t *, unsigned int);
void uv_sem_post(uv_sem_t *);
int uv_sem_trywait(uv_sem_t *);
void uv_sem_wait(uv_sem_t *);
int uv_thread_create(uv_thread_t *, uv_thread_cb, void *);
int uv_thread_equal(const uv_thread_t *, const uv_thread_t *);
int uv_thread_join(uv_thread_t *);
uv_thread_t uv_thread_self(void);
__END_DECLS

typedef struct {
  int __hack;
} uv_loop_t;

typedef enum {
  UV_RUN_DEFAULT = 0,
  UV_RUN_ONCE,
  UV_RUN_NOWAIT,
} uv_run_mode;

// Handle types.

typedef struct {
  _Alignas(__max_align_t) int __hack;
} uv_handle_t;

typedef struct {
  _Alignas(__max_align_t) int __hack;
} uv_idle_t;

typedef struct {
  _Alignas(__max_align_t) int __hack;
} uv_pipe_t;

typedef struct {
  _Alignas(__max_align_t) int __hack;
} uv_poll_t;

#if 0
union uv_any_handle {
  uv_async_t async;
  uv_check_t check;
  uv_handle_t handle;
  uv_idle_t idle;
  uv_pipe_t pipe;
  uv_prepare_t prepare;
  uv_stream_t stream;
  uv_tcp_t tcp;
  uv_timer_t timer;
  uv_udp_t udp;

};

enum uv_poll_event {
  UV_READABLE = 0x1,
  UV_WRITABLE = 0x2,
  UV_DISCONNECT = 0x4,
};

typedef struct {
  _Alignas(__max_align_t) int __hack;
} uv_prepare_t;

typedef struct {
  _Alignas(__max_align_t) int __hack;
} uv_stream_t;

typedef struct {
  _Alignas(__max_align_t) int __hack;
} uv_write_t;

// Request types.

typedef void (*uv_alloc_cb)(uv_handle_t *, size_t, uv_buf_t *);
typedef void (*uv_close_cb)(uv_handle_t *);
typedef void (*uv_idle_cb)(uv_idle_t *);
typedef void (*uv_poll_cb)(uv_poll_t *, int, int);
typedef void (*uv_prepare_cb)(uv_prepare_t *);
typedef void (*uv_read_cb)(uv_stream_t *, ssize_t, const uv_buf_t *);
typedef void (*uv_walk_cb)(uv_handle_t *, void *);
typedef void (*uv_write_cb)(uv_write_t *, int);

__BEGIN_DECLS
void uv_cancel(uv_req_t *);
void uv_close(uv_handle_t *, uv_close_cb);
uv_loop_t *uv_default_loop(void);  // TODO(ed): Remove this?
int uv_fileno(const uv_handle_t *, uv_os_fd_t *);
size_t uv_handle_size(uv_handle_type);
int uv_has_ref(const uv_handle_t *);
int uv_loop_alive(const uv_loop_t *);
int uv_loop_close(uv_loop_t *);
int uv_loop_init(uv_loop_t *);
size_t uv_loop_size(void);
int uv_idle_init(uv_loop_t *, uv_idle_t *);
int uv_idle_stop(uv_idle_t *);
int uv_idle_start(uv_idle_t *, uv_idle_cb);
int uv_is_active(const uv_handle_t *);
int uv_is_closing(const uv_handle_t *);
uint64_t uv_now(const uv_loop_t *);
int uv_pipe_init(uv_loop_t *, uv_pipe_t *, int);
int uv_pipe_open(uv_pipe_t *, uv_file);
int uv_poll_init(uv_loop_t *, uv_poll_t *, int);
int uv_poll_init_socket(uv_loop_t *, uv_poll_t *, uv_os_sock_t);
int uv_poll_start(uv_poll_t *, int, uv_poll_cb);
int uv_poll_stop(uv_poll_t *);
int uv_prepare_init(uv_loop_t *, uv_prepare_t *);
int uv_prepare_start(uv_prepare_t *, uv_prepare_cb);
int uv_prepare_stop(uv_prepare_t *);
int uv_read_start(uv_stream_t *, uv_alloc_cb, uv_read_cb);
void uv_ref(uv_handle_t *);
int uv_run(uv_loop_t *, uv_run_mode);
void uv_stop(uv_loop_t *);
int uv_tcp_open(uv_tcp_t*, uv_os_sock_t);
void uv_update_time(uv_loop_t *);
void uv_unref(uv_handle_t *);
void uv_walk(uv_loop_t *, uv_walk_cb, void *);
int uv_write(uv_write_t *, uv_stream_t *, const uv_buf_t *, size_t,
             uv_write_cb);
__END_DECLS
#endif

#endif
