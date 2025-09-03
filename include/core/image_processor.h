#pragma once
#include <vector>
#include <string>
#include <functional>
#include <memory>

namespace imagetui
{
  namespace core
  {

    using ProgressCallback = std::function<void(float)>;

    struct ImageData
    {
      int width;
      int height;
      int channels;
      std::vector<unsigned char> data;

      ImageData() : width(0), height(0), channels(0) {}
      ImageData(int w, int h, int c) : width(w), height(h), channels(c)
      {
        data.resize(static_cast<size_t>(w) * h * c);
      }

      bool isValid() const
      {
        return width > 0 && height > 0 && channels > 0 && !data.empty();
      }

      size_t getPixelIndex(int x, int y) const
      {
        return static_cast<size_t>(y * width + x) * channels;
      }
    };

    class ImageProcessor
    {
    public:
      // Fast memory-mapped loading
      static std::unique_ptr<ImageData> loadImageFast(const std::string &filename);
      static std::unique_ptr<ImageData> loadImage(const std::string &filename);

      // Ultra-fast BMP saving (no compression)
      static bool saveImageUltraFast(const std::string &filename, const ImageData &image);
      // Fast saving with quality control
      static bool saveImageFast(const std::string &filename, const ImageData &image, int quality = 85);
      // Regular saving
      static bool saveImage(const std::string &filename, const ImageData &image);

      static std::vector<std::string> getSupportedFormats();

    private:
      static bool validateImage(const ImageData &image);
      static std::vector<unsigned char> readFileToBuffer(const std::string &filename);
      static bool writeBufferToFile(const std::string &filename, const std::vector<unsigned char> &buffer);
    };

    void sendProgress(const ProgressCallback &callback, float progress);

  }
}