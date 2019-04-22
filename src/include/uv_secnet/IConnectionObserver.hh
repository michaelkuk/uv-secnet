/**
 * Copyright (c) 2019 Michael Kuk
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <string>
#include "Buffer.hh"

namespace uv_secnet {
  class IConnectionObserver {
    public:
      virtual ~IConnectionObserver() {};
      virtual void onConnectionData(buffer_ptr_t data);
      virtual void onConnectionError(std::string);
      virtual void onConnectionFinished();
      virtual void onConnectionClosed();
      virtual void onConnectionDestroyed();
      virtual void onConnectionOpen();
  };
}