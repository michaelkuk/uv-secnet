/**
 * Copyright (c) 2019 Michael Kuk
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include<openssl/ssl.h>

#include "IConnectionObserver.hh"

namespace uv_secnet
{
  class TLSConnection : private IConnectionObserver {
    public:
      ~TLSConnection();

      void close();
      void destroy();
      void write(char*, size_t);
    
      static std::shared_ptr<TLSConnection> create(uv_stream_t* uvStream, IConnectionObserver* observer);

      private:
        
  };
} // namespace uv_secnet
