/**
 * Copyright (c) 2019 Michael Kuk
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */



#pragma once

#include <memory>
#include <uv.h>

namespace uv_secnet {
  class Buffer {
    public:
      char* base;
      int len;

      Buffer(const uv_buf_t* uvBuf);
      Buffer(char* base, int len);
      ~Buffer();

      static std::shared_ptr<Buffer> makeShared(const uv_buf_t* uvBuf);
      static std::shared_ptr<Buffer> makeShared(char* base, int len);
    private:
      Buffer();
  };

  using buffer_ptr_t = std::shared_ptr<Buffer>;
}