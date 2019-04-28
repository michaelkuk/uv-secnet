#include "uv_secnet/TCPClient.hh"

using namespace uv_secnet;

TCPClient::TCPClient(uv_loop_t* loop, std::string address, IClientObserver* obs)
  : observer(obs),
    loop(loop),
    tlsEnabled(false),
    clientError(IClient::ERR::NONE),
    clientStatus(IClient::STATUS::NOT_CONNECTED)
{
  uv_tcp_init(loop, &tcpHandle);
  uv_timer_init(loop, &timer);
  url = Url::parse(address);
  hints = safe_alloc<addrinfo>();
  hints->ai_family = AF_INET;
}

TCPClient::~TCPClient()
{
  free(hints);
}

void TCPClient::connect()
{
  if (clientStatus != IClient::STATUS::CONNECTING && clientStatus != IClient::STATUS::CONNECTED) {
    performConnect();
  }
}

void TCPClient::send(buffer_ptr_t data)
{
  if (
    clientConnectionStatus != IClient::CONNECTION_STATUS::CLOSED &&
    clientConnectionStatus != IClient::CONNECTION_STATUS::DONE &&
    clientConnectionStatus != IClient::CONNECTION_STATUS::PENDING 
  )
  {
    connection->write(data);
  }
}

void TCPClient::disconnect()
{
  if (connection != nullptr) {
    connection->destroy();
  }

  if (clientStatus == IClient::STATUS::WAITING_FOR_RECONNECT) {
    uv_timer_stop(&timer);
  }

  clientStatus = IClient::STATUS::NOT_CONNECTED;
  observer->onClientStatusChanged(clientStatus);
}

void TCPClient::close()
{
  connection->close();
}

IClient::STATUS TCPClient::status()
{
  return clientStatus;
}

IClient::ERR TCPClient::lastError()
{
  return clientError;
}

IClient::CONNECTION_STATUS TCPClient::connectionStatus()
{
  return clientConnectionStatus;
}

void TCPClient::enableTLS()
{
  enableTLS(TLSContext::getDefault());
}

void TCPClient::enableTLS(std::shared_ptr<TLSContext> ctx)
{
  tlsContext = ctx;
}


void TCPClient::onConnectionData(buffer_ptr_t data)
{
  observer->onClientData(data);
}

void TCPClient::onConnectionError(std::string msg)
{
  clientConnectionStatus = IClient::CONNECTION_STATUS::DONE;
  connection = nullptr;

  int reconnect = observer->onClientError(IClient::ERR::WRITE_TIMEOUT, msg);

  if (reconnect < 0) {
    clientStatus = IClient::STATUS::ERROR;
  } else if (reconnect == 0) {
    performConnect();
  } else {
    uv_timer_start(&timer, on_reconnect_timer_tick, reconnect, 0);
    clientStatus = IClient::STATUS::WAITING_FOR_RECONNECT;
    observer->onClientStatusChanged(clientStatus);
  }
}

void TCPClient::onConnectionFinished()
{
  clientStatus = IClient::STATUS::NOT_CONNECTED;
  clientConnectionStatus = IClient::CONNECTION_STATUS::DONE;
  observer->onClientConnectionStatusChanged(clientConnectionStatus);
  observer->onClientStatusChanged(clientStatus);
}

void TCPClient::onConnectionClosed()
{
  clientConnectionStatus = IClient::CONNECTION_STATUS::CLOSED;
  observer->onClientConnectionStatusChanged(clientConnectionStatus);
}

void TCPClient::onConnectionDestroyed()
{
  clientConnectionStatus = IClient::CONNECTION_STATUS::FINISHED;
  observer->onClientConnectionStatusChanged(clientConnectionStatus);
}

void TCPClient::onConnectionOpen()
{
  clientConnectionStatus = IClient::CONNECTION_STATUS::OPEN;
  observer->onClientConnectionStatusChanged(clientConnectionStatus);

  clientStatus = IClient::STATUS::CONNECTED;
  observer->onClientStatusChanged(clientStatus);
}

void TCPClient::performConnect()
{
  if (isIPAddress(url->host)) {
    connectByIP();
  } else {
    resolveFQDN();
  }
}

void TCPClient::connectByIP()
{
  if (isIPv4(url->host)) {
    uv_ip4_addr(url->host.c_str(), url->port, reinterpret_cast<sockaddr_in*>(&address));
  } else {
    uv_ip6_addr(url->host.c_str(), url->port, reinterpret_cast<sockaddr_in6*>(&address));
  }

  connectByIP(&address);
}

void TCPClient::connectByIP(sockaddr* addr)
{
  auto req = safe_alloc<uv_connect_t>();
  req->data = this;

  clientStatus = IClient::STATUS::CONNECTING;
  observer->onClientStatusChanged(clientStatus);

  uv_tcp_connect(req, &tcpHandle, addr, on_connect_cb);
}

void TCPClient::resolveFQDN()
{
  auto req = safe_alloc<uv_getaddrinfo_t>();
  req->data = this;
  uv_getaddrinfo(loop, req, on_get_addr_info, url->host.c_str(), NULL, hints);
}

void TCPClient::onConnect(uv_stream_t* stream, int status)
{
  if (status != 0) {
    clientStatus = IClient::STATUS::ERROR;
    clientError = IClient::ERR::CONNECTION_FAILED;
    observer->onClientError(IClient::ERR::CONNECTION_FAILED, std::string(uv_err_name(status)));
    observer->onClientStatusChanged(clientStatus);
    return;
  }

  connection = TCPConnection::create(stream);

  if (tlsContext != nullptr) {
    connection = tlsContext->secureClientConnection(connection, url->host);
  }

  connection->initialize(this);
}

void TCPClient::onFQDNResolved(int status, addrinfo* res)
{
  if (status != 0) {
    clientError = IClient::ERR::DNS_RESOLUTION_FAILED;
    clientStatus = IClient::STATUS::NOT_CONNECTED;

    observer->onClientError(clientError, std::string(uv_err_name(status)));
    observer->onClientStatusChanged(clientStatus);
  } else {
    uv_ip4_addr(
      inet_ntoa(reinterpret_cast<sockaddr_in*>(res->ai_addr)->sin_addr),
      url->port, reinterpret_cast<sockaddr_in*>(&address)
    );

    connectByIP(&address);
  }
  
  uv_freeaddrinfo(res);
}

void TCPClient::on_connect_cb(uv_connect_t* req, int status)
{
  auto tcpClient = reinterpret_cast<TCPClient*>(req->data);

  tcpClient->onConnect(reinterpret_cast<uv_stream_t*>(req->handle), status);

  free(req);
}

void TCPClient::on_reconnect_timer_tick(uv_timer_t* handle)
{
  auto tcpClient = reinterpret_cast<TCPClient*>(handle->data);
  tcpClient->performConnect();
}

void TCPClient::on_get_addr_info(uv_getaddrinfo_t* req, int status, struct addrinfo* res)
{
  auto tcpClient = reinterpret_cast<TCPClient*>(req->data);

  tcpClient->onFQDNResolved(status, res);

  free(req);
}

