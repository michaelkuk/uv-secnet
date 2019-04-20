#include "uv_secnet.hh"

#include <iostream>
#include <functional>
#include <string>
#include <sstream>
#include <uv.h>
#include <memory.h>
#include <memory>
#include <unistd.h>


typedef struct {
  int counter = 0;
  std::shared_ptr<uv_secnet::TCPConnection> conn = 0;
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

    ctx->conn->write(data, sl);
  }
}

void connectCb(uv_connect_t* req, int status) {
  if(status != 0) {
    std::cout << "Connection Error: " << uv_err_name(status) << std::endl;
  } else {
    std::cout << "Connection Established" << std::endl;
    auto ctx = (secnet_ctx_t*) req->data;

    ctx->conn = uv_secnet::TCPConnection::create((uv_stream_t*)req->handle, ctx->obs);
    uv_timer_start(ctx->timer, timerTick, 500, 500);
  }

  free(req);
}

void runLoop() {
  uv_loop_t* loop = uv_default_loop();
  secnet_ctx_t* ctx = (secnet_ctx_t*)malloc(sizeof(secnet_ctx_t));

  initCtx(ctx);

  uv_ip4_addr("127.0.0.1", 8989, ctx->addr);
  uv_connect_t* cReq = (uv_connect_t*)malloc(sizeof(uv_connect_t));
  cReq->data = ctx;

  uv_tcp_connect(cReq, ctx->tcpHandle, (sockaddr*)ctx->addr, connectCb);

  uv_run(loop, UV_RUN_DEFAULT);

  freeCtx(ctx);
}


int main () {
  std::cout << "hello, world!" << std::endl;

  runLoop();

  usleep(1000000);
  return 0;
}