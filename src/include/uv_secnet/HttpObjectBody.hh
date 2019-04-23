#pragma once

#include <string>
#include <memory>

#include "Buffer.hh"

namespace uv_secnet
{
  class HTTPObjectBody {
    public:
      static std::shared_ptr<HTTPObjectBody> create(std::string type, buffer_ptr_t body);

      std::string& getType();
      unsigned long getLength();
      char* getPayload();

    private:
      HTTPObjectBody(std::string type, buffer_ptr_t body);
      HTTPObjectBody();
      std::string type;
      buffer_ptr_t body;
  };

  typedef std::shared_ptr<HTTPObjectBody> http_body_ptr_t;
} // namespace uv_secnet
