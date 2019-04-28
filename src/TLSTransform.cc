#include "uv_secnet/TLSTransform.hh"

using namespace uv_secnet;

bool TLSTransform::debug = false;

TLSTransform::TLSTransform(std::shared_ptr<IConnection> connection, SSL* ssl)
  : connection(connection),
    ssl(ssl),
    in_bio(BIO_new(BIO_s_mem())),
    out_bio(BIO_new(BIO_s_mem())),
    observer(nullptr),
    handshakeDone(false),
    clientMode(true)
{
  BIO_set_mem_eof_return(in_bio, -1);
  BIO_set_mem_eof_return(out_bio, -1);
  SSL_set_bio(ssl, in_bio, out_bio);
}

// IConnectionObserver implementation
void TLSTransform::onConnectionData(buffer_ptr_t data)
{
  if (!handshakeDone) {
    return handleHandshake(data);
  }
  
  int written = BIO_write(in_bio, data->base, data->len);

  if (written > 0) {
    auto buf = Buffer::makeShared(safe_alloc<char>(written), written);
    int read = SSL_read(ssl, buf->base, buf->len);

    buf->len = read;

    observer->onConnectionData(buf);
  } else {
    observer->onConnectionError("Failed to write to SSL bio");
    destroy();
  }

}

void TLSTransform::onConnectionError(std::string err)
{
  this->observer->onConnectionError(err);
}

void TLSTransform::onConnectionFinished()
{
  observer->onConnectionFinished();
}

void TLSTransform::onConnectionClosed()
{
  observer->onConnectionClosed();
}

void TLSTransform::onConnectionDestroyed()
{
  observer->onConnectionDestroyed();
}

void TLSTransform::onConnectionOpen()
{
  if (clientMode) {
    SSL_set_connect_state(ssl);
    handleHandshake();
  } else {
    SSL_set_accept_state(ssl);
  }
}


// IConnection implementation
void TLSTransform::close()
{
  connection->close();
}

void TLSTransform::destroy()
{
  connection->destroy();
}

void TLSTransform::write(buffer_ptr_t data)
{
  SSL_write(ssl, data->base, data->len);

  int pending = BIO_ctrl_pending(out_bio);

  if (pending) {
    auto buf = Buffer::makeShared(safe_alloc<char>(pending), pending);
    BIO_read(out_bio, buf->base, buf->len);
    connection->write(buf);
  } else {
    observer->onConnectionError("SSL Write failed");
    destroy();
  }
}

void TLSTransform::initialize(IConnectionObserver* obs)
{
  if (TLSTransform::debug) {
    setDebugMode();
  }

  observer = obs;
  connection->initialize(this);
}

void TLSTransform::handleHandshake(buffer_ptr_t data)
{
  int written = BIO_write(in_bio, data->base, data->len);
  handleHandshake();
}

void TLSTransform::handleHandshake()
{
  int res = SSL_do_handshake(ssl);

  if (res == 1) {
    handshakeDone = true;
    observer->onConnectionOpen();
  } else if (res == 0) {
    flushErrors();
  } else {
    int err = SSL_get_error(ssl, res);

    if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE){
      flushErrors();
    } else {
      flushWriteBio();
    }
  }
}

void TLSTransform::flushErrors()
{
  std::string errors("");
  unsigned long lastCode = ERR_get_error();

  while (lastCode) {
    char* msg = safe_alloc<char>(1024);
    ERR_error_string_n(lastCode, msg, 1024);
    errors = errors + std::string(msg) + std::string("\n");
    lastCode = ERR_get_error();
  }

  observer->onConnectionError(errors);
  destroy();
}

void TLSTransform::flushWriteBio()
{
  int pending = BIO_ctrl_pending(out_bio);

  if (pending <= 0) {
    return;
  }

  auto buf = Buffer::makeShared(safe_alloc<char>(pending), pending);
  BIO_read(out_bio, buf->base, buf->len);
  connection->write(buf);
}

TLSTransform::~TLSTransform()
{
  SSL_free(ssl);
}

void TLSTransform::setClientMode()
{
  clientMode = true;
}

void TLSTransform::setDebugMode()
{
  SSL_set_info_callback(ssl, info_callback);
}
