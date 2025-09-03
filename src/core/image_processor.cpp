#include "core/image_processor.h"
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <algorithm>

namespace imagetui
{
  namespace core
  {

    std::unique_ptr<ImageData> ImageProcessor::loadImage(const std::string &filename)
    {
      cv::Mat mat = cv::imread(filename, cv::IMREAD_COLOR | cv::IMREAD_IGNORE_ORIENTATION);

      if (mat.empty())
      {
        std::cerr << "Error: Could not load image: " << filename << std::endl;
        return nullptr;
      }

      return std::make_unique<ImageData>(std::move(mat));
    }

    bool ImageProcessor::saveImageFast(const std::string &filename, const ImageData &image, int quality)
    {
      if (!image.isValid())
      {
        std::cerr << "Error: Invalid image data" << std::endl;
        return false;
      }

      auto params = getCompressionParams(filename, quality);

      bool success = cv::imwrite(filename, image.getMat(), params);

      if (!success)
      {
        std::cerr << "Error: Could not save image: " << filename << std::endl;
      }

      return success;
    }

    bool ImageProcessor::saveImageUltraFast(const std::string &filename, const ImageData &image)
    {
      if (!image.isValid())
      {
        std::cerr << "Error: Invalid image data" << std::endl;
        return false;
      }

      auto params = getUltraFastParams(filename);

      bool success = cv::imwrite(filename, image.getMat(), params);

      if (!success)
      {
        std::cerr << "Error: Could not save image: " << filename << std::endl;
      }

      return success;
    }

    std::vector<int> ImageProcessor::getCompressionParams(const std::string &filename, int quality)
    {
      std::string ext = filename.substr(filename.find_last_of('.') + 1);
      std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

      if (ext == "jpg" || ext == "jpeg")
      {
        return {cv::IMWRITE_JPEG_QUALITY, std::clamp(quality, 0, 100),
                cv::IMWRITE_JPEG_OPTIMIZE, 1};
      }
      else if (ext == "png")
      {
        int compression = (quality > 80) ? 1 : 0;
        return {cv::IMWRITE_PNG_COMPRESSION, compression};
      }
      else if (ext == "webp")
      {
        return {cv::IMWRITE_WEBP_QUALITY, std::clamp(quality, 1, 100)};
      }
      else if (ext == "bmp")
      {
        return {};
      }

      return {};
    }

    std::vector<int> ImageProcessor::getUltraFastParams(const std::string &filename)
    {
      std::string ext = filename.substr(filename.find_last_of('.') + 1);
      std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

      if (ext == "jpg" || ext == "jpeg")
      {
        return {cv::IMWRITE_JPEG_QUALITY, 95, cv::IMWRITE_JPEG_OPTIMIZE, 0};
      }
      else if (ext == "png")
      {
        return {cv::IMWRITE_PNG_COMPRESSION, 0};
      }
      else if (ext == "webp")
      {

        return {cv::IMWRITE_WEBP_QUALITY, 90};
      }
      else if (ext == "bmp")
      {
        return {};
      }

      return {};
    }

  }
}