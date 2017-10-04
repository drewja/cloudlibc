// Copyright (c) 2017 Nuxi, https://nuxi.nl/
//
// This file is distributed under a 2-clause BSD license.
// See the LICENSE file for details.

#ifndef UV_UV_IMPL_H
#define UV_UV_IMPL_H

#include <uv.h>

static inline void __uv_handle_init(uv_loop_t *loop, uv_handle_t *handle,
                                    uv_handle_type type) {
  handle->loop = loop;
  handle->type = type;
}

static inline void __uv_stream_init(uv_loop_t *loop, uv_stream_t *handle,
                                    uv_handle_type type) {
  __uv_handle_init(loop, (uv_handle_t *)handle, type);
  handle->write_queue_size = 0;
}

#endif
