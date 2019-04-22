/**
 * Copyright (c) 2019 Michael Kuk
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include "IConnection.hh"
#include "IConnectionObserver.hh"

namespace uv_secnet
{
  class IConnectionTransform : public IConnection, public IConnectionObserver {
    public:
      virtual ~IConnectionTransform() {};
  };
} // namespace uv_secnet
