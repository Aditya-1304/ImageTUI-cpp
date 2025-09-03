#include <iostream>
#include <chrono>
#include <iomanip>
#include "core/image_processor.h"
#include "filters/basic.h"

using namespace imagetui;

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    std::cout << "Usage: " << argv[0] << " <input_image> <output_image>" << std::endl;
    return 1;
  }

  std::string input_file = argv[1];
  std::string output_file = argv[2];

  auto total_start = std::chrono::high_resolution_clock::now();

  std::cout << " Loading image: " << input_file << std::endl;
  auto load_start = std::chrono::high_resolution_clock::now();

  auto image = core::ImageProcessor::loadImageFast(input_file);
  if (!image)
  {
    std::cerr << " Failed to load image: " << input_file << std::endl;
    return 1;
  }

  auto load_end = std::chrono::high_resolution_clock::now();
  auto load_duration = std::chrono::duration_cast<std::chrono::milliseconds>(load_end - load_start);

  std::cout << " Image loaded (" << load_duration.count() << "ms)" << std::endl;
  std::cout << "    Dimensions: " << image->width << "x" << image->height << std::endl;
  std::cout << "    Channels: " << image->channels << std::endl;

  long long pixels = static_cast<long long>(image->width) * image->height;
  bool is_large_image = pixels > 5000000; // 5 megapixels threshold

  if (is_large_image)
  {
    std::cout << "    Large image detected - will use  BMP saving" << std::endl;
  }

  // auto progress_callback = [](float progress)
  // {
  //   int percent = static_cast<int>(progress * 100);
  //   int bar_width = 30;
  //   int filled = static_cast<int>(bar_width * progress);

  //   std::cout << "\r🔄 Processing: [";
  //   for (int i = 0; i < bar_width; ++i)
  //   {
  //     if (i < filled)
  //       std::cout << "█";
  //     else
  //       std::cout << "░";
  //   }
  //   std::cout << "] " << std::setw(3) << percent << "%";
  //   std::flush(std::cout);
  // };

  std::cout << "  Converting to grayscale..." << std::endl;
  auto process_start = std::chrono::high_resolution_clock::now();

  auto result = filters::BasicFilters::grayscale(*image);

  auto process_end = std::chrono::high_resolution_clock::now();
  auto process_duration = std::chrono::duration_cast<std::chrono::milliseconds>(process_end - process_start);

  std::cout << "\n Processing completed (" << process_duration.count() << "ms)" << std::endl;

  if (!result)
  {
    std::cerr << " Failed to process image" << std::endl;
    return 1;
  }

  std::cout << "Saving image: " << output_file << std::endl;
  auto save_start = std::chrono::high_resolution_clock::now();

  bool save_success;
  if (is_large_image)
  {
    std::cout << "Using  BMP format..." << std::endl;
    save_success = core::ImageProcessor::saveImageUltraFast(output_file, *result);
  }
  else
  {
    std::cout << "   Using optimized format..." << std::endl;
    save_success = core::ImageProcessor::saveImageFast(output_file, *result, 80);
  }

  if (!save_success)
  {
    std::cerr << " Failed to save image" << std::endl;
    return 1;
  }

  auto save_end = std::chrono::high_resolution_clock::now();
  auto save_duration = std::chrono::duration_cast<std::chrono::milliseconds>(save_end - save_start);

  auto total_end = std::chrono::high_resolution_clock::now();
  auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(total_end - total_start);

  std::cout << "Image saved (" << save_duration.count() << "ms)" << std::endl;

  std::cout << "   Timing Summary:" << std::endl;
  std::cout << "   Load:    " << std::setw(4) << load_duration.count() << "ms (mmap)" << std::endl;
  std::cout << "   Process: " << std::setw(4) << process_duration.count() << "ms (parallel)" << std::endl;

  if (is_large_image)
  {
    std::cout << "   Save:    " << std::setw(4) << save_duration.count() << "ms (BMP)" << std::endl;
  }
  else
  {
    std::cout << "   Save:    " << std::setw(4) << save_duration.count() << "ms (normal)" << std::endl;
  }

  std::cout << "   Total:   " << std::setw(4) << total_duration.count() << "ms" << std::endl;

  // Calculate performance metrics
  double mp_per_sec = (pixels / 1000000.0) / (process_duration.count() / 1000.0);

  std::cout << " Performance: " << std::fixed << std::setprecision(2)
            << mp_per_sec << " MP/sec" << std::endl;

  return 0;
}