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

namespace uv_secnet
{
  class TLSContext {
    public:
      ~TLSContext();

      static std::shared_ptr<TLSContext> getDefault();
      static std::shared_ptr<TLSContext> get(std::string& name);
      static void shutdown();

    private:
      static std::shared_ptr<TLSContext> defaultCtx;
      static std::map<std::string, std::shared_ptr<TLSContext>> ctxMap;
  };
} // namespace uv_secnet
