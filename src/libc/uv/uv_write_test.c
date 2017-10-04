// Copyright (c) 2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#include <testing.h>
#include <unistd.h>
#include <uv.h>

// Use a state number to validate that callbacks are actually run.
static void write_cb1_success(uv_write_t *req, int status) {
  ASSERT_EQ(1, ++*(int *)req->data);
}

static void write_cb2_success(uv_write_t *req, int status) {
  ASSERT_EQ(2, ++*(int *)req->data);
}

static void close_cb_success(uv_handle_t *handle) {
  ASSERT_EQ(3, ++*(int *)handle->data);
}

TEST(uv_write, example) {
  uv_loop_t loop;
  ASSERT_EQ(0, uv_loop_init(&loop));

  // Attach to a pipe.
  int fds[2];
  ASSERT_EQ(0, pipe(fds));
  uv_pipe_t out;
  ASSERT_EQ(0, uv_pipe_init(&loop, &out, 0));
  ASSERT_EQ(0, uv_pipe_open(&out, fds[0]));

  // Schedule that data is written into the pipe.
  uv_buf_t bufs1[2] = {
      uv_buf_init((char *)"Hello ", 6),
      uv_buf_init((char *)"World!\n", 7),
  };
  uv_write_t req1;
  int state = 0;
  req1.data = &state;
  ASSERT_EQ(0,
            uv_write(&req1, (uv_stream_t *)&out, bufs1, 2, write_cb1_success));

  uv_buf_t bufs2[2] = {
      uv_buf_init((char *)"Goodbye ", 8),
      uv_buf_init((char *)"World!\n", 7),
  };
  uv_write_t req2;
  req2.data = &state;
  ASSERT_EQ(0,
            uv_write(&req2, (uv_stream_t *)&out, bufs2, 2, write_cb2_success));

  // Schedule that the pipe is closed.
  out.data = &state;
  uv_close((uv_handle_t *)&out, close_cb_success);

  // Execute.
  ASSERT_EQ(0, state);
  ASSERT_EQ(0, uv_run(&loop, UV_RUN_DEFAULT));
  ASSERT_EQ(3, state);
  ASSERT_EQ(0, uv_loop_close(&loop));

  // Extract data from the other end of the pipe.
  char buf[29];
  ASSERT_EQ(28, read(fds[1], buf, sizeof(buf)));
  ASSERT_ARREQ("Hello World!\nGoodbye World!\n", buf, 28);
  ASSERT_EQ(0, close(fds[1]));
}
