#include "uv_secnet.hh"

#include <iostream>
#include <functional>
#include <string>
#include <sstream>
#include <uv.h>
#include <memory.h>
#include <memory>
#include <http_parser.h>


typedef struct {
  int counter = 0;
  std::shared_ptr<uv_secnet::IConnection> conn = 0;
  uv_timer_t* timer = nullptr;
  uv_tcp_t* tcpHandle = nullptr;
  sockaddr_in* addr = nullptr;
  uv_secnet::IConnectionObserver* obs = nullptr;
} secnet_ctx_s;

typedef secnet_ctx_s secnet_ctx_t;

void freeHandle(uv_handle_t* handle) {
  free(handle);
}

void initCtx (secnet_ctx_t* ctx) {
  ctx->counter = 0;
  ctx->conn = nullptr;
  ctx->timer = (uv_timer_t*)malloc(sizeof(uv_timer_t));
  ctx->tcpHandle = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
  ctx->addr = (sockaddr_in*)malloc(sizeof(sockaddr_in));
  ctx->obs = new uv_secnet::IConnectionObserver();

  uv_timer_init(uv_default_loop(), ctx->timer);
  uv_tcp_init(uv_default_loop(), ctx->tcpHandle);

  ctx->timer->data = ctx;
  ctx->tcpHandle->data = ctx;
}

void freeCtx(secnet_ctx_t* ctx) {
  free(ctx->timer);
  uv_close((uv_handle_t*) ctx->tcpHandle, freeHandle);
  ctx->conn = nullptr;
  free(ctx->addr);
  delete ctx->obs;
  free(ctx);
}

void timerTick(uv_timer_t* handle) {
  auto ctx = (secnet_ctx_t*) handle->data;

  if(++ctx->counter == 20) {
    std::cout << "Reached final tick, stopping counter" << std::endl;

    uv_timer_stop(handle);
    ctx->conn->close();
  } else {
    std::cout << "Processing tick #" << ctx->counter << std::endl;
    std::stringstream ss;

    ss << "Hello from C++ tick #" << ctx->counter << std::endl;
    auto sd = ss.str();
    int sl = sd.length();

    char* data = (char*)malloc(sl + 1);

    strcpy(data, sd.c_str());

    ctx->conn->write(uv_secnet::Buffer::makeShared(data, sl+1));
  }
}

void connectCb(uv_connect_t* req, int status) {
  if(status != 0) {
    std::cout << "Connection Error: " << uv_err_name(status) << std::endl;
  } else {
    auto ctx = (secnet_ctx_t*) req->data;

    auto ssl_ctx = uv_secnet::TLSContext::getDefault();

    // ctx->conn = ssl_ctx->secureClientConnection(uv_secnet::TCPConnection::create((uv_stream_t*)req->handle), "google.com");
    ctx->conn = uv_secnet::TCPConnection::create((uv_stream_t*)req->handle);
    ctx->conn->initialize(ctx->obs);

    uv_secnet::HTTPObject obj("http://locahost:9999/post");
    obj.setHeader("Connection", "Upgrade")
      ->setHeader("Upgrade", "websocket")
      ->setHeader("Sec-WebSocket-Key", "2BMqVIxuwA32Yuh1ydRutw==")
      ->setHeader("Sec-WebSocket-Version", "13");
    
    ctx->conn->write(obj.toBuffer());
    ctx->conn->close();
  }

  free(req);
}

void runLoop() {
  uv_loop_t* loop = uv_default_loop();
  secnet_ctx_t* ctx = (secnet_ctx_t*)malloc(sizeof(secnet_ctx_t));

  initCtx(ctx);

  uv_ip4_addr("127.0.0.1", 9999, ctx->addr);
  // uv_ip4_addr("216.58.201.110", 443, ctx->addr);
  uv_connect_t* cReq = (uv_connect_t*)malloc(sizeof(uv_connect_t));
  cReq->data = ctx;

  uv_tcp_connect(cReq, ctx->tcpHandle, (sockaddr*)ctx->addr, connectCb);

  uv_run(loop, UV_RUN_DEFAULT);

  freeCtx(ctx);
}

void testUri(std::string url) {
  auto uri = uv_secnet::Url::parse(url);
  return;
}

void on_addr_info(uv_getaddrinfo_t* req, int status, struct addrinfo* res)
{
  auto i4 = AF_INET;
  auto i6 = AF_INET6;

  if (status != 0) {
    char e[1024];

    uv_err_name_r(status, e, 1024);
    std::cout << e << std::endl;
    return;
  }

  if (res->ai_family == AF_INET) {
    std::cout << inet_ntoa(((sockaddr_in*)res->ai_addr)->sin_addr) << 
      ":" <<
      ((sockaddr_in*)res->ai_addr)->sin_port << std::endl;
  }
  std::cout << "DONE!" << std::endl;
}

class LogObserver : public uv_secnet::IClientObserver {
  public:
    uv_secnet::HTTPObject* o;
    uv_secnet::TCPClient* c;

    LogObserver() : o(nullptr), c(nullptr) {};

    virtual void onClientStatusChanged(uv_secnet::IClient::STATUS status) {
      std::cout << "Client status changed" << std::endl;
    };
        // <0 - do not reconnect, 0 - reconnect now, >0 - reconnect delay
    virtual int onClientError(uv_secnet::IClient::ERR, std::string err)
    {
      std::cout << "Error: " << err << std::endl;
    }

    virtual void onClientData(uv_secnet::buffer_ptr_t data)
    {
      // std::cout << "Got data::" << std::endl << std::string(data->base, data->len) << std::endl;
    }

    virtual void onClientConnectionStatusChanged(uv_secnet::IClient::CONNECTION_STATUS status)
    {
      if (status == uv_secnet::IClient::CONNECTION_STATUS::OPEN) {
        std::cout << "Connected" << std::endl;
        c->send(o->toBuffer());
        c->close();
      }
      std::cout << "Connection status changed" << std::endl;
    }

};

int main () {
  // std::string s("https://admin:fickmich@localhost:443/something?query=string");

  // runLoop();

  // auto buf = uv_secnet::safe_alloc<char>(16);
  // uv_secnet::randomBytes(buf, 16);
  // auto data = uv_secnet::vendor::base64_encode((unsigned char*)buf, 16);

  // std::string body("Text payload, yeah baby!");
  // auto buf = uv_secnet::Buffer::makeShared(const_cast<char*>(body.c_str()), body.length());
  
  // uv_secnet::HTTPObject obj(s);
  // obj.createBody("text/plain", buf)
  //   ->setHeader("X-Auth-Token", "trlalala")
  //   ->setHeader("User-Agent", "fucking_C++_baby!");

  // auto b = obj.toBuffer();

  // // std::cout << std::string(d, one.length() + two.length());
  // std::cout << std::string(b->base, b->len);
  // std::cout << data << std::endl;

  uv_loop_t* loop = uv_default_loop();

  // uv_secnet::TLSTransform::debug = true;

  std::string host("https://google.com:443/");
  LogObserver o;

  uv_secnet::TCPClient client(loop, host, &o);
  auto httpO = uv_secnet::HTTPObject(host);
  httpO.setHeader("User-Agent", "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Ubuntu Chromium/73.0.3683.86 Chrome/73.0.3683.86 Safari/537.36");
  httpO.setHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8");

  o.o = &httpO;
  o.c = &client;

  client.enableTLS();
  client.connect();

  uv_run(loop, UV_RUN_DEFAULT);

  return 0;
}