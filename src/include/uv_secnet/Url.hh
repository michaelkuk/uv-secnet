#pragma once

#include <memory>
#include <http_parser.h>
#include "util.hh"

namespace uv_secnet
{
  class Url {
    public:
      std::string url;
      std::string scheme;
      std::string host;
      unsigned int port;
      std::string path;
      std::string query;
      std::string fragment;
      std::string userinfo;

      bool hasScheme;
      bool hasHost;
      bool hasPort;
      bool hasPath;
      bool hasQuery;
      bool hasFragment;
      bool hasUserinfo;

      static std::shared_ptr<Url> parse(std::string);

    private:
      Url();
  };
} // namespace uv_secnet
