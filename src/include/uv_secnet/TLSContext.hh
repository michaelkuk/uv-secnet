/**
 * Copyright (c) 2019 Michael Kuk
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <memory>
#include <string>
#include <map>

#include <openssl/err.h>
#include <openssl/dh.h>
#include <openssl/ssl.h>
#include <openssl/conf.h>
#include <openssl/engine.h>

#include "IConnection.hh"
#include "TLSTransform.hh"

namespace uv_secnet
{
  class TLSContext {
    public:
      ~TLSContext();

      static std::shared_ptr<TLSContext> getDefault();
      static std::shared_ptr<TLSContext> get(std::string& name);
      static void shutdown();

      std::shared_ptr<IConnection> secureClientConnection(std::shared_ptr<IConnection> conn);
      std::shared_ptr<IConnection> secureClientConnection(std::shared_ptr<IConnection> conn, std::string hostname);

      void doNotValidateCert();

    private:
      SSL_CTX* ssl_ctx;
      static std::shared_ptr<TLSContext> defaultCtx;
      static std::map<std::string, std::shared_ptr<TLSContext>> ctxMap;

      static std::shared_ptr<TLSContext> makeContext();
      static void initializeSSL();

      static bool sslInitialized;

      TLSContext();
  };
} // namespace uv_secnet
