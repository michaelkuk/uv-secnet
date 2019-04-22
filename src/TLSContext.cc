/**
 * Copyright (c) 2019 Michael Kuk
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "uv_secnet/TLSContext.hh"

using namespace uv_secnet;

bool TLSContext::sslInitialized = false;
std::shared_ptr<TLSContext> TLSContext::defaultCtx = nullptr;

std::shared_ptr<TLSContext> TLSContext::getDefault()
{
  if (!TLSContext::sslInitialized) {
    TLSContext::initializeSSL();
  }

  if (TLSContext::defaultCtx == nullptr) {
    TLSContext::defaultCtx = TLSContext::makeContext();
  }

  return TLSContext::defaultCtx;
}

std::shared_ptr<TLSContext> TLSContext::get(std::string& name)
{
  return nullptr;
}

TLSContext::TLSContext()
{
  ssl_ctx = SSL_CTX_new(SSLv23_method());
}

void TLSContext::doNotValidateCert()
{
  SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_NONE, NULL);
}

std::shared_ptr<IConnection> TLSContext::secureClientConnection(std::shared_ptr<IConnection> conn)
{
  auto trans = new TLSTransform(conn, SSL_new(ssl_ctx));
  trans->setClientMode();

  return std::shared_ptr<TLSTransform>(trans);
}

std::shared_ptr<IConnection> TLSContext::secureClientConnection(std::shared_ptr<IConnection> conn, std::string hostname)
{
  auto ssl = SSL_new(ssl_ctx);
  SSL_set_tlsext_host_name(ssl, hostname.c_str());
  auto trans = new TLSTransform(conn, ssl);
  trans->setClientMode();

  return std::shared_ptr<TLSTransform>(trans);
}

void TLSContext::shutdown()
{
  ERR_remove_state(0);
  ENGINE_cleanup();
  CONF_modules_unload(1);
  ERR_free_strings();
  EVP_cleanup();
  sk_SSL_COMP_free(SSL_COMP_get_compression_methods());
  CRYPTO_cleanup_all_ex_data();
}

void TLSContext::initializeSSL()
{
  SSL_library_init();
  SSL_load_error_strings();
  ERR_load_BIO_strings();
  OpenSSL_add_all_algorithms();

  TLSContext::sslInitialized = true;
}

std::shared_ptr<TLSContext> TLSContext::makeContext()
{
  return std::shared_ptr<TLSContext>(new TLSContext());
}


TLSContext::~TLSContext()
{
  if (ssl_ctx != nullptr) {
    SSL_CTX_free(ssl_ctx);
  }
}