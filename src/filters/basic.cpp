#include "filters/basic.h"
#include <opencv2/imgproc.hpp>
#include <chrono>
#include <iostream>

namespace imagetui
{
  namespace filters
  {

    std::unique_ptr<core::ImageData> BasicFilters::grayscale(const core::ImageData &input)
    {
      if (!input.isValid())
        return nullptr;

      auto t0 = std::chrono::high_resolution_clock::now();

      cv::Mat result;
      const cv::Mat &src = input.getMat();

      if (src.channels() == 3 || src.channels() == 4)
      {
        cv::Mat gray;
        cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
        cv::cvtColor(gray, result, cv::COLOR_GRAY2BGR);
      }
      else if (src.channels() == 1)
      {
        cv::cvtColor(src, result, cv::COLOR_GRAY2BGR);
      }
      else
      {
        result = src.clone();
      }

      auto t1 = std::chrono::high_resolution_clock::now();
      auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
      std::cout << "Grayscale: " << ms << "ms" << std::endl;

      return std::make_unique<core::ImageData>(std::move(result));
    }

  }
}