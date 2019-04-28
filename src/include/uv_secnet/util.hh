/**
 * Copyright (c) 2019 Michael Kuk
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <memory.h>
#include <string>
#include <random>
#include <arpa/inet.h>

#include "vendor/base64.hh"

namespace uv_secnet
{
  template<class T>
  T* safe_alloc() {
    T* ptr = static_cast<T*>(malloc(sizeof(T)));
    memset(ptr, 0, sizeof(T));
    return ptr;
  }

  template<class T>
  T* safe_alloc(size_t size) {
    T* ptr = static_cast<T*>(malloc(size));
    memset(ptr, 0, size);
    return ptr;
  }

  static std::string CRLF("\r\n");

  inline std::string toUpperCase(std::string& str) {
    std::string ret;

    for(char& c : str) {
      ret += std::toupper(c);      
    }

    return ret;
  }

  inline void randomBytes(void* buffer, size_t size) {
    std::random_device rd;
    std::uniform_int_distribution<int> dist(0,255);

    char* b = reinterpret_cast<char*>(buffer);

    for(int i = 0; i < size; i++) {
      b[i] = static_cast<char>(dist(rd) & 0xFF);
    }
  }

  inline bool isIPv4(const std::string& str)
  {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, str.c_str(), &(sa.sin_addr)) != 0;
  }

  inline bool isIPv6(const std::string& str)
  {
    struct sockaddr_in6 sa;
    return inet_pton(AF_INET6, str.c_str(), &(sa.sin6_addr)) != 0;
  }

  inline bool isIPAddress(const std::string& str)
  {
    return isIPv4(str) || isIPv6(str);
  }

} // namespace uv_secnet
