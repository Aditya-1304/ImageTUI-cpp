#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include "core/image_processor.h"
#include <iostream>

namespace imagetui
{
  namespace core
  {
    std::unique_ptr<ImageData> ImageProcessor::loadImage(const std::string &filename)
    {
      int width, height, channels;
      unsigned char *data = stbi_load(filename.c_str(), &width, &height, &channels, 0);

      if (!data)
      {
        std::cerr << "Failed to load: " << stbi_failure_reason() << std::endl;

        return nullptr;
      }

      auto image = std::make_unique<ImageData>(width, height, channels);
      std::copy(data, data + (width * height * channels), image->data.begin());
      stbi_image_free(data);

      return image;
    }

    bool ImageProcessor::saveImage(const std::string &filename, const ImageData &image)
    {
      std::string ext = filename.substr(filename.find_last_of('.') + 1);

      if (ext == "png")
      {
        return stbi_write_png(filename.c_str(), image.width, image.height, image.channels, image.data.data(), image.width * image.channels);
      }
      else if (ext == "jpg")
      {
        return stbi_write_jpg(filename.c_str(), image.width, image.height,
                              image.channels, image.data.data(), 90);
      }
      return false;
    }

  }
}