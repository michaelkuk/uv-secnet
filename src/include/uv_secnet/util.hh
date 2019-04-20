/**
 * Copyright (c) 2019 Michael Kuk
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

#include <memory.h>

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
} // namespace uv_secnet
