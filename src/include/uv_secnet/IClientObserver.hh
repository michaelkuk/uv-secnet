#pragma once

#include <string>

#include "IClient.hh"
#include "Buffer.hh"

namespace uv_secnet
{
  class IClientObserver {
    public:
      virtual void onClientStatusChanged(IClient::STATUS) = 0;
      // <0 - do not reconnect, 0 - reconnect now, >0 - reconnect delay
      virtual int onClientError(IClient::ERR, std::string) = 0;
      virtual void onClientData(buffer_ptr_t) = 0;
      virtual void onClientConnectionStatusChanged(IClient::CONNECTION_STATUS) = 0;
  };
} // namespace uv_secnet
