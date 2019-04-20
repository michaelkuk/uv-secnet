/**
 * Copyright (c) 2019 Michael Kuk
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#include "uv_secnet/Buffer.hh"
#include <iostream>

using namespace uv_secnet;

Buffer::Buffer(const uv_buf_t* uvBuf)
  : base(uvBuf->base),
    len(uvBuf->len)
{}

Buffer::Buffer(char* base, int len)
  : base(base),
    len(len)
{}

Buffer::~Buffer()
{
  free(this->base);
}

std::shared_ptr<Buffer> Buffer::makeShared(const uv_buf_t* uvBuf)
{
  return std::make_shared<Buffer>(uvBuf);
}

std::shared_ptr<Buffer> Buffer::makeShared(char* base, int len)
{
  return std::make_shared<Buffer>(base, len);
}
