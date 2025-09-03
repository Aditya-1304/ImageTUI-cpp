#pragma once
#include "core/image_processor.h"

namespace imagetui
{
  namespace filters
  {
    class BasicFilters
    {
    public:
      static std::unique_ptr<core::ImageData> grayscale(const core::ImageData &input);
    };
  }
}