#pragma once
#include "core/image_processor.h"

namespace imagetui
{
  namespace filters
  {
    class ArtisticFilters
    {
    public:
      static std::unique_ptr<core::ImageData> sepia(const core::ImageData &input);
      static std::unique_ptr<core::ImageData> vignette(const core::ImageData &input, float strength = 0.8f);
      static std::unique_ptr<core::ImageData> noise(const core::ImageData &input, int strength = 25);
      static std::unique_ptr<core::ImageData> oilPainting(const core::ImageData &input, int radius = 3, int intensity = 20);

    private:
      static void horizontalOilPass(const cv::Mat &src, cv::Mat &dst, int radius, int intensity);
      static void verticalOilPass(const cv::Mat &src, cv::Mat &dst, int radius, int intensity);
      static int findDominantIntensity(const std::vector<int> &counts);
      static void setOilPixel(cv::Mat &dst, int row, int col, int maxIdx,
                              const std::vector<int> &counts,
                              const std::vector<float> &avgR,
                              const std::vector<float> &avgG,
                              const std::vector<float> &avgB);
    };
  }
}