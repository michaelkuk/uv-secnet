#pragma once

#include <memory>
#include <string>
#include <arpa/inet.h>
#include <uv.h>

#include "util.hh"
#include "Url.hh"
#include "IClient.hh"
#include "IClientObserver.hh"
#include "IConnection.hh"
#include "IConnectionObserver.hh"
#include "TLSContext.hh"
#include "TCPConnection.hh"

namespace uv_secnet
{
  class TCPClient : public IClient, protected IConnectionObserver {
    public:
      TCPClient(uv_loop_t*, std::string, IClientObserver*);
      ~TCPClient();
      virtual void connect() override;
      virtual void send(buffer_ptr_t) override;
      virtual void disconnect() override;
      virtual void close() override;
      virtual IClient::STATUS status() override;
      virtual IClient::ERR lastError() override;
      virtual IClient::CONNECTION_STATUS connectionStatus() override;
      void enableTLS();
      void enableTLS(std::shared_ptr<TLSContext>);

    protected:
      virtual void onConnectionData(buffer_ptr_t data) override;
      virtual void onConnectionError(std::string) override;
      virtual void onConnectionFinished() override;
      virtual void onConnectionClosed() override;
      virtual void onConnectionDestroyed() override;
      virtual void onConnectionOpen() override;

    private:
      TCPClient();

      IClientObserver* observer;
      uv_loop_t* loop;
      bool tlsEnabled;
      IClient::ERR clientError;
      IClient::STATUS clientStatus;
      IClient::CONNECTION_STATUS clientConnectionStatus;
      std::shared_ptr<Url> url;
      std::shared_ptr<IConnection> connection;
      uv_tcp_t tcpHandle;
      uv_timer_t timer;
      std::shared_ptr<TLSContext> tlsContext;
      addrinfo* hints;
      sockaddr address;

      void performConnect();
      void connectByIP();
      void connectByIP(sockaddr*);
      void resolveFQDN();

      void onConnect(uv_stream_t*, int status);
      void onFQDNResolved(int status, addrinfo* res);

      static void on_connect_cb(uv_connect_t*, int status);
      static void on_reconnect_timer_tick(uv_timer_t*);
      static void on_get_addr_info(uv_getaddrinfo_t* req, int status, struct addrinfo* res);
  };
} // namespace uv_secnet
