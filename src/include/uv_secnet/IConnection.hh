#pragma once

#include <memory>
#include "Buffer.hh"
#include "IConnectionObserver.hh"

namespace uv_secnet
{
  class IConnection {
    public:
      virtual ~IConnection() {};
      virtual void close() = 0;
      virtual void destroy() = 0;
      virtual void write(buffer_ptr_t) = 0;
      virtual void initialize(IConnectionObserver*) = 0;
  };
} // namespace uv_secnet
