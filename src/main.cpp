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
    std::cerr << "Usage: " << argv[0] << " <input_image> <output_image>" << std::endl;
    return 1;
  }

  std::string input_file = argv[1];
  std::string output_file = argv[2];

  auto total_start = std::chrono::high_resolution_clock::now();

  std::cout << "Loading: " << input_file << std::endl;
  auto load_start = std::chrono::high_resolution_clock::now();

  auto image = core::ImageProcessor::loadImage(input_file);
  if (!image)
  {
    std::cerr << "Failed to load image!" << std::endl;
    return 1;
  }

  auto load_end = std::chrono::high_resolution_clock::now();
  auto load_ms = std::chrono::duration_cast<std::chrono::milliseconds>(load_end - load_start).count();
  std::cout << "Loaded (" << load_ms << "ms)" << std::endl;

  std::cout << "Converting to grayscale..." << std::endl;
  auto process_start = std::chrono::high_resolution_clock::now();

  auto result = filters::BasicFilters::grayscale(*image);
  if (!result)
  {
    std::cerr << "Failed to process image!" << std::endl;
    return 1;
  }

  auto process_end = std::chrono::high_resolution_clock::now();
  auto process_ms = std::chrono::duration_cast<std::chrono::milliseconds>(process_end - process_start).count();
  std::cout << "Processed (" << process_ms << "ms)" << std::endl;

  std::cout << "Saving: " << output_file << std::endl;
  auto save_start = std::chrono::high_resolution_clock::now();

  bool success = core::ImageProcessor::saveImageUltraFast(output_file, *result);
  if (!success)
  {
    std::cerr << "Failed to save image!" << std::endl;
    return 1;
  }

  auto save_end = std::chrono::high_resolution_clock::now();
  auto save_ms = std::chrono::duration_cast<std::chrono::milliseconds>(save_end - save_start).count();
  std::cout << "Saved (" << save_ms << "ms)" << std::endl;

  auto total_end = std::chrono::high_resolution_clock::now();
  auto total_ms = std::chrono::duration_cast<std::chrono::milliseconds>(total_end - total_start).count();

  long long pixels = 1LL * image->width() * image->height();
  double mp_per_sec = (pixels / 1000000.0) / (total_ms / 1000.0);

  std::cout << "Load: " << load_ms << "ms | Process: " << process_ms
            << "ms | Save: " << save_ms << "ms" << std::endl;
  std::cout << "Total: " << total_ms << "ms" << std::endl;
  std::cout << "Speed: " << std::fixed << std::setprecision(1)
            << mp_per_sec << " MP/sec" << std::endl;

  return 0;
}