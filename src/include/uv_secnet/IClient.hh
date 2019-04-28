#pragma once

#include "Buffer.hh"

namespace uv_secnet
{
  class IClient {
    public:
      enum STATUS {
        NOT_CONNECTED,
        CONNECTING,
        CONNECTED,
        ERROR,
        WAITING_FOR_RECONNECT,
      };

      enum ERR {
        NONE,
        DNS_RESOLUTION_FAILED,
        CONNECTION_FAILED,
        WRITE_TIMEOUT,
      };

      enum CONNECTION_STATUS {
        PENDING,
        OPEN,
        CLOSED,
        FINISHED,
        DONE,
      };

      virtual void connect() = 0;
      virtual void send(buffer_ptr_t) = 0;
      virtual void close() = 0;
      virtual void disconnect() = 0;
      virtual IClient::STATUS status() = 0;
      virtual IClient::ERR lastError() = 0;
      virtual IClient::CONNECTION_STATUS connectionStatus() = 0;
  };
} // namespace uv_secnet
