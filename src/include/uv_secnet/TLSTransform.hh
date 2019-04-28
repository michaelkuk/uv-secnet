/**
 * Copyright (c) 2019 Michael Kuk
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <openssl/err.h>
#include <openssl/dh.h>
#include <openssl/ssl.h>
#include <openssl/conf.h>
#include <openssl/engine.h>

#include "util.hh"
#include "IConnectionTransform.hh"

namespace uv_secnet
{
  class TLSTransform : public IConnectionTransform {
    public:
      TLSTransform(std::shared_ptr<IConnection> connection, SSL* ssl);
      virtual ~TLSTransform() override;

      // IConnectionObserver implementation
      virtual void onConnectionData(buffer_ptr_t data) override;
      virtual void onConnectionError(std::string) override;
      virtual void onConnectionFinished() override;
      virtual void onConnectionClosed() override;
      virtual void onConnectionDestroyed() override;
      virtual void onConnectionOpen() override;

      // IConnection implementation
      virtual void close() override;
      virtual void destroy() override;
      virtual void write(buffer_ptr_t) override;
      virtual void initialize(IConnectionObserver*) override;

      void setClientMode();
      void setDebugMode();

      static bool debug;

    private:
      std::shared_ptr<IConnection> connection;
      SSL* ssl;
      BIO* in_bio;
      BIO* out_bio;
      IConnectionObserver* observer;
      bool handshakeDone;
      bool clientMode;

      void handleHandshake(buffer_ptr_t);
      void handleHandshake();
      void flushErrors();
      void flushWriteBio();

      static void info_callback(const SSL* ssl, int where, int ret) {
        if(ret == 0) {
          printf("-- ssl_info_callback: error occured.\n");
          return;
        }

        auto SSL_WHERE_INFO = [&](int w, int flag, std::string msg) {
          if(w & flag) {
            printf("%20.20s", msg.c_str());
            printf(" - %30.30s ", SSL_state_string_long(ssl));
            printf(" - %5.10s ", SSL_state_string(ssl));
            printf("\n");
          }                       
        };
      
        SSL_WHERE_INFO(where, SSL_CB_LOOP, "LOOP");
        SSL_WHERE_INFO(where, SSL_CB_HANDSHAKE_START, "HANDSHAKE START");
        SSL_WHERE_INFO(where, SSL_CB_HANDSHAKE_DONE, "HANDSHAKE DONE");
      }
  };
} // namespace uv_secnet
