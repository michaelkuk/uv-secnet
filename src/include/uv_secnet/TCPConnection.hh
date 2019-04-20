/**
 * Copyright (c) 2019 Michael Kuk
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <uv.h>

#include "IConnectionObserver.hh"
#include "util.hh"

namespace uv_secnet {
  class TCPConnection {
    public:
      typedef std::function<uv_buf_t* (uv_buf_t*, TCPConnection*)> write_transform_t;
      typedef std::function<buffer_ptr_t (buffer_ptr_t, TCPConnection*)> read_transform_t;

      ~TCPConnection();

      virtual void close();
      virtual void destroy();
      virtual void write(char*, size_t);
    
      static std::shared_ptr<TCPConnection> create(uv_stream_t* uvStream, IConnectionObserver* observer);

    protected:
      bool isClosed;
      bool isFinished;
      uv_stream_t* uvStream;
      IConnectionObserver* observer;

      std::vector<write_transform_t> writeTransforms;
      std::vector<read_transform_t> readTransforms;

      TCPConnection();
      TCPConnection(uv_stream_t* uvStream, IConnectionObserver* observer);

      virtual void onData(buffer_ptr_t);
      virtual void onFinish();
      virtual void onClose();
      virtual void onWrite(int);
      virtual void onDestroy();

      static void read_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
      static void read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf);
      static void write_cb(uv_write_t*, int);
      static void shutdown_cb(uv_shutdown_t*, int);
      static void close_cb(uv_handle_t*);
  };

  typedef struct {
    TCPConnection* connection;
    uv_buf_t* buffer;
  } tcp_write_ctx_s;

  typedef tcp_write_ctx_s tcp_write_ctx_t;
}