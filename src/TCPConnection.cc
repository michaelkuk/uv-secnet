/**
 * Copyright (c) 2019 Michael Kuk
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "uv_secnet/TCPConnection.hh"
#include <iostream>

using namespace uv_secnet;

TCPConnection::TCPConnection(uv_stream_t* uvStream)
  : isClosed(false),
    isFinished(false),
    uvStream(uvStream),
    observer(nullptr)
{
  uvStream->data = this;
}

std::shared_ptr<TCPConnection> TCPConnection::create(uv_stream_t* uvStream)
{
  return std::shared_ptr<TCPConnection>(new TCPConnection(uvStream));
}

TCPConnection::~TCPConnection()
{
  if (uvStream != nullptr) {
    uvStream->data = nullptr;
    destroy();
  }
}

void TCPConnection::initialize(IConnectionObserver* obs)
{
  observer = obs;
  uv_read_start(uvStream, TCPConnection::read_alloc_cb, TCPConnection::read_cb);
  observer->onConnectionOpen();
}

void TCPConnection::write(buffer_ptr_t data)
{ 
  uv_buf_t* buff = safe_alloc<uv_buf_t>();
  buff->base = safe_alloc<char>(data->len);
  buff->len = data->len;

  memcpy(buff->base, data->base, data->len);

  tcp_write_ctx_t* writeCtx = safe_alloc<tcp_write_ctx_t>();
  writeCtx->connection = this;
  writeCtx->buffer = buff;

  uv_write_t* writeReq = safe_alloc<uv_write_t>();
  writeReq->data = writeCtx;

  uv_write(writeReq, uvStream, buff, 1, write_cb);
}

void TCPConnection::close()
{
  uv_shutdown_t* req = safe_alloc<uv_shutdown_t>();
  req->data = this;

  uv_shutdown(req, uvStream, shutdown_cb);
}

void TCPConnection::destroy()
{
  uv_read_stop(uvStream);
  uv_close(reinterpret_cast<uv_handle_t*>(uvStream), close_cb);
}

void TCPConnection::onData(buffer_ptr_t data)
{
  observer->onConnectionData(data);
}

void TCPConnection::onFinish()
{
  isFinished = true;
  observer->onConnectionFinished();

  if (isClosed) {
    destroy();
  }
}

void TCPConnection::onClose()
{
  isClosed = true;

  if (isFinished) {
    destroy();
  }
}

void TCPConnection::onWrite(int status)
{
  if (status != 0) {
    onError(status);
  }
}

void TCPConnection::onError(int status)
{
  observer->onConnectionError(std::string(uv_err_name(status)));
}

void TCPConnection::onDestroy()
{
  observer->onConnectionDestroyed();
  uvStream = nullptr;
}

void TCPConnection::read_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
  buf->base = safe_alloc<char>(suggested_size);
  buf->len = suggested_size;
}

void TCPConnection::read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf)
{
  TCPConnection* conn = static_cast<TCPConnection*>(stream->data);

  if (nread > 0) {
    auto data = Buffer::makeShared(buf);
    data->len = nread;
    conn->onData(data);
  } else {
    conn->onFinish();
    free(buf->base);
  }
}

void TCPConnection::write_cb(uv_write_t* req, int status)
{
  auto writeCtx = static_cast<tcp_write_ctx_t*>(req->data);
  writeCtx->connection->onWrite(status);
  free(writeCtx->buffer->base);
  free(writeCtx->buffer);
  free(writeCtx);
  free(req);
}

void TCPConnection::shutdown_cb(uv_shutdown_t* req, int status)
{
  TCPConnection* conn = static_cast<TCPConnection*>(req->data);
  conn->onClose();

  free(req);
}

void TCPConnection::close_cb(uv_handle_t* handle)
{
  if (handle->data != nullptr) {
    TCPConnection* conn = static_cast<TCPConnection*>(handle->data);

    conn->onDestroy();
  }
}