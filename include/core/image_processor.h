#pragma once
#include <opencv2/core.hpp>
#include <string>
#include <functional>
#include <memory>
#include <vector>

namespace imagetui
{
  namespace core
  {

    using ProgressCallback = std::function<void(float)>;

    struct ImageData
    {
      cv::Mat mat;

      ImageData() = default;
      explicit ImageData(cv::Mat m) : mat(std::move(m)) {}
      ImageData(int width, int height, int type) : mat(height, width, type) {}

      bool isValid() const { return !mat.empty(); }
      int width() const { return mat.cols; }
      int height() const { return mat.rows; }
      int channels() const { return mat.channels(); }

      cv::Mat &getMat() { return mat; }
      const cv::Mat &getMat() const { return mat; }
    };

    class ImageProcessor
    {
    public:
      static std::unique_ptr<ImageData> loadImage(const std::string &filename);
      static bool saveImageFast(const std::string &filename, const ImageData &image, int quality = 85);
      static bool saveImageUltraFast(const std::string &filename, const ImageData &image);

    private:
      static std::vector<int> getCompressionParams(const std::string &filename, int quality);
      static std::vector<int> getUltraFastParams(const std::string &filename);
    };

  }
}