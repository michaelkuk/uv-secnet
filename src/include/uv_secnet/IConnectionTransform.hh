/**
 * Copyright (c) 2019 Michael Kuk
 * 
 * This software is released under the MIT License.
 * https://opensource.org/licenses/MIT
 */

#pragma once

namespace uv_secnet
{
  class IConnectionTransform {
    public:
      virtual ~IConnectionTransform() = 0;
      virtual void initialize() = 0;
      virtual void transformWrite() = 0;
      virtual void transformRead() = 0;
      virtual void finalizeRead() = 0;
      virtual void closeWrite() = 0;
  };
} // namespace uv_secnet
