#include "filters/basic.h"
#include <thread>
#include <future>
#include <algorithm>

namespace imagetui
{
  namespace filters
  {
    std::unique_ptr<core::ImageData> BasicFilters::grayscale(
        const core::ImageData &input)
    {
      if (!input.isValid())
      {
        return nullptr;
      }

      auto output = std::make_unique<core::ImageData>(input.width, input.height, 3);

      const int num_threads = std::thread::hardware_concurrency();
      const int rows_per_thread = input.height / num_threads;

      std::vector<std::future<void>> futures;

      for (int thread_id = 0; thread_id < num_threads; ++thread_id)
      {
        int start_row = thread_id * rows_per_thread;
        int end_row = (thread_id == num_threads - 1) ? input.height : (thread_id + 1) * rows_per_thread;

        if (start_row >= input.height)
          break;

        futures.push_back(std::async(std::launch::async, [&input, &output, start_row, end_row]()
                                     {
          for (int y = start_row; y < end_row; ++y) {
            for (int x = 0; x < input.width; ++x) {
              size_t input_idx = input.getPixelIndex(x,y);
              size_t output_idx = output->getPixelIndex(x,y);

              unsigned char gray_value;

              if (input.channels >= 3) {
                unsigned char red = input.data[input_idx];
                unsigned char green = input.data[input_idx + 1];
                unsigned char blue = input.data[input_idx + 2];
                gray_value = static_cast<unsigned char>(0.299f * red + 0.587f * green + 0.114f * blue);
              } else {
                gray_value = input.data[input_idx];
              }

              output->data[output_idx] = gray_value;
              output->data[output_idx + 1] = gray_value;
              output->data[output_idx + 2] = gray_value;

            }
          } }));
      }

      for (auto &future : futures)
      {
        future.wait();
      }

      return output;
    }
  }
}