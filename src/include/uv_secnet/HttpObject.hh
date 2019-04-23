#pragma once

#include <map>
#include <string>
#include <sstream>
#include <memory.h>

#include "Buffer.hh"
#include "HttpObjectBody.hh"
#include "Url.hh"

namespace uv_secnet
{
  class HTTPObject {
    public:
      HTTPObject(std::string url);
      HTTPObject(std::shared_ptr<Url>);

      buffer_ptr_t toBuffer();

      HTTPObject* setMethod(std::string);
      HTTPObject* setHeader(std::string, std::string);

      HTTPObject* createBody(std::string contentType, buffer_ptr_t payload);
    private:
      HTTPObject();
      std::string method;
      std::shared_ptr<Url> url;
      http_body_ptr_t body;
      std::map<std::string, std::string> headers;

      void encodeHeaders(std::ostringstream&);
  };
} // namespace uv_secnet
