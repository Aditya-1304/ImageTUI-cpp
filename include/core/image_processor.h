#pragma once

#include <vector>
#include <string>
#include <functional>
#include <memory>

namespace imagetui
{
  namespace core
  {
    struct ImageData
    {
      int width;
      int height;
      int channels;
      std::vector<unsigned char> data;

      ImageData() : width(0), height(0), channels(0) {}
      ImageData(int w, int h, int c) : width(w), height(h), channels(c)
      {
        data.resize(w * h * c);
      }

      bool isValid() const
      {
        return width > 0 && height > 0 && channels > 0 && !data.empty();
      }

      size_t getPixelIndex(int x, int y) const
      {
        return (y * width + x) * channels;
      }
    };

    class ImageProcessor
    {
    public:
      static std::unique_ptr<ImageData> loadImage(const std::string &filename);
      static bool saveImage(const std::string &filename, const ImageData &image);
    };
  }
}