#include "filters/artistic.h"
#include <opencv2/imgproc.hpp>
#include <chrono>
#include <iostream>
#include <random>
#include <algorithm>
#include <cmath>

namespace imagetui
{
  namespace filters
  {
    std::unique_ptr<core::ImageData> ArtisticFilters::sepia(const core::ImageData &input)
    {
      if (!input.isValid())
        return nullptr;

      auto t0 = std::chrono::high_resolution_clock::now();

      const cv::Mat &src = input.getMat();
      cv::Mat result;
      src.copyTo(result);

      cv::parallel_for_(cv::Range(0, src.rows), [&](const cv::Range &range)
                        {
        for (int y = range.start; y < range.end; y++) {
          const cv::Vec3b * src_row = src.ptr<cv::Vec3b>(y);
          cv::Vec3b * dst_row = result.ptr<cv::Vec3b>(y);

          for (int x = 0; x < src.cols; x++) {
            const cv::Vec3b& pixel = src_row[x];
            float blue = pixel[0];
            float green = pixel[1];
            float red = pixel[2];

            float new_red = red * 0.393f + green * 0.769f + blue * 0.189f;
            float new_green = red * 0.349f + green * 0.686f + blue * 0.168f;
            float new_blue = red * 0.272f + green * 0.534f + blue * 0.131f;

            dst_row[x][0] = std::min(255.0f, new_blue);
            dst_row[x][1] = std::min(255.0f, new_green);
            dst_row[x][2] = std::min(255.0f, new_red);

          }
        } });

      auto t1 = std::chrono::high_resolution_clock::now();
      auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
      std::cout << "Sepia: " << ms << "ms" << std::endl;

      return std::make_unique<core::ImageData>(std::move(result));
    }
  }
}