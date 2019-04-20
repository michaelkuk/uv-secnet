/**
 * Copyright (c) 2019 Michael Kuk
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "Buffer.hh"

namespace uv_secnet {
  class IConnectionObserver {
    public:
      virtual void onConnectionData(buffer_ptr_t data);
      virtual void onConnectionError();
      virtual void onConnectionFinished();
      virtual void onConnectionClosed();
      virtual void onConnectionDestroyed();
      virtual void onConnectionOpen();
  };
}