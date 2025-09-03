#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include "core/image_processor.h"

#include <iostream>
#include <algorithm>
#include <cctype>
#include <fstream>

#ifdef __linux__
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#define USE_MMAP 1
#else
#define USE_MMAP 0
#endif

namespace imagetui
{
  namespace core
  {

    std::unique_ptr<ImageData> ImageProcessor::loadImageFast(const std::string &filename)
    {
#if USE_MMAP
      int fd = open(filename.c_str(), O_RDONLY);
      if (fd == -1)
      {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return loadImage(filename);
      }

      struct stat sb;
      if (fstat(fd, &sb) == -1)
      {
        close(fd);
        return loadImage(filename);
      }

      if (sb.st_size == 0)
      {
        close(fd);
        std::cerr << "Empty file: " << filename << std::endl;
        return nullptr;
      }

      void *mapped = mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
      if (mapped == MAP_FAILED)
      {
        close(fd);
        return loadImage(filename);
      }

      madvise(mapped, sb.st_size, MADV_SEQUENTIAL | MADV_WILLNEED);

      int width, height, channels;
      unsigned char *data = stbi_load_from_memory(
          static_cast<unsigned char *>(mapped), sb.st_size,
          &width, &height, &channels, 0);

      munmap(mapped, sb.st_size);
      close(fd);

      if (!data)
      {
        std::cerr << "Failed to decode image: " << stbi_failure_reason() << std::endl;
        return nullptr;
      }

      auto image = std::make_unique<ImageData>();
      image->width = width;
      image->height = height;
      image->channels = channels;
      image->data.resize(static_cast<size_t>(width) * height * channels);

      std::copy(data, data + (static_cast<size_t>(width) * height * channels), image->data.begin());

      stbi_image_free(data);

      std::cout << "Loaded image (mmap): " << width << "x" << height << " with " << channels << " channels" << std::endl;
      return image;
#else

      return loadImage(filename);
#endif
    }

    std::unique_ptr<ImageData> ImageProcessor::loadImage(const std::string &filename)
    {
      auto buffer = readFileToBuffer(filename);
      if (buffer.empty())
      {
        return nullptr;
      }

      int width, height, channels;
      unsigned char *data = stbi_load_from_memory(
          buffer.data(), static_cast<int>(buffer.size()),
          &width, &height, &channels, 0);

      if (!data)
      {
        std::cerr << "Failed to decode image: " << stbi_failure_reason() << std::endl;
        return nullptr;
      }

      auto image = std::make_unique<ImageData>();
      image->width = width;
      image->height = height;
      image->channels = channels;
      image->data.resize(static_cast<size_t>(width) * height * channels);

      std::copy(data, data + (static_cast<size_t>(width) * height * channels), image->data.begin());

      stbi_image_free(data);

      std::cout << "Loaded image: " << width << "x" << height << " with " << channels << " channels" << std::endl;
      return image;
    }

    bool ImageProcessor::saveImageFast(const std::string &filename, const ImageData &image, int quality)
    {
      if (!validateImage(image))
      {
        std::cerr << "Invalid image data" << std::endl;
        return false;
      }

      std::string ext = filename.substr(filename.find_last_of('.') + 1);
      std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

      std::vector<unsigned char> output_buffer;
      output_buffer.reserve(image.data.size());

      auto write_callback = [](void *context, void *data, int size)
      {
        auto *buffer = static_cast<std::vector<unsigned char> *>(context);
        auto *bytes = static_cast<unsigned char *>(data);
        buffer->insert(buffer->end(), bytes, bytes + size);
      };

      int result = 0;

      if (ext == "png")
      {

        stbi_write_png_compression_level = 1;
        result = stbi_write_png_to_func(write_callback, &output_buffer,
                                        image.width, image.height, image.channels,
                                        image.data.data(), image.width * image.channels);
      }
      else if (ext == "jpg" || ext == "jpeg")
      {
        result = stbi_write_jpg_to_func(write_callback, &output_buffer,
                                        image.width, image.height, image.channels,
                                        image.data.data(), quality);
      }
      else if (ext == "bmp")
      {
        result = stbi_write_bmp_to_func(write_callback, &output_buffer,
                                        image.width, image.height, image.channels,
                                        image.data.data());
      }
      else if (ext == "tga")
      {
        result = stbi_write_tga_to_func(write_callback, &output_buffer,
                                        image.width, image.height, image.channels,
                                        image.data.data());
      }
      else
      {
        std::cerr << "Unsupported format: " << ext << std::endl;
        return false;
      }

      if (result == 0 || output_buffer.empty())
      {
        std::cerr << "Failed to encode image" << std::endl;
        return false;
      }

      return writeBufferToFile(filename, output_buffer);
    }

    bool ImageProcessor::saveImage(const std::string &filename, const ImageData &image)
    {
      return saveImageFast(filename, image, 90);
    }

    std::vector<std::string> ImageProcessor::getSupportedFormats()
    {
      return {"png", "jpg", "jpeg", "bmp", "tga"};
    }

    bool ImageProcessor::validateImage(const ImageData &image)
    {
      return image.isValid() &&
             image.data.size() == static_cast<size_t>(image.width * image.height * image.channels);
    }

    std::vector<unsigned char> ImageProcessor::readFileToBuffer(const std::string &filename)
    {
      std::ifstream file(filename, std::ios::binary | std::ios::ate);
      if (!file.is_open())
      {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return {};
      }

      std::streamsize file_size = file.tellg();
      if (file_size <= 0)
      {
        std::cerr << "Invalid file size: " << filename << std::endl;
        return {};
      }

      file.seekg(0, std::ios::beg);

      std::vector<unsigned char> buffer(static_cast<size_t>(file_size));
      if (!file.read(reinterpret_cast<char *>(buffer.data()), file_size))
      {
        std::cerr << "Failed to read file: " << filename << std::endl;
        return {};
      }

      return buffer;
    }

    bool ImageProcessor::writeBufferToFile(const std::string &filename, const std::vector<unsigned char> &buffer)
    {
      std::ofstream file(filename, std::ios::binary);
      if (!file.is_open())
      {
        std::cerr << "Cannot create file: " << filename << std::endl;
        return false;
      }

      file.write(reinterpret_cast<const char *>(buffer.data()), buffer.size());

      if (!file.good())
      {
        std::cerr << "Failed to write file: " << filename << std::endl;
        return false;
      }

      std::cout << "Image saved: " << filename << std::endl;
      return true;
    }

    void sendProgress(const ProgressCallback &callback, float progress)
    {
      if (callback)
      {
        callback(progress);
      }
    }

    bool ImageProcessor::saveImageUltraFast(const std::string &filename, const ImageData &image)
    {
      if (!validateImage(image))
      {
        std::cerr << "Invalid image data" << std::endl;
        return false;
      }

      // Force BMP extension for ultra-fast saving
      std::string bmp_filename = filename;
      size_t dot_pos = bmp_filename.find_last_of('.');
      if (dot_pos != std::string::npos)
      {
        bmp_filename = bmp_filename.substr(0, dot_pos) + ".bmp";
      }
      else
      {
        bmp_filename += ".bmp";
      }

      const uint32_t width = static_cast<uint32_t>(image.width);
      const uint32_t height = static_cast<uint32_t>(image.height);
      const uint32_t row_size = width * 3;
      const uint32_t padding = (4 - (row_size % 4)) % 4;
      const uint32_t padded_row_size = row_size + padding;
      const uint32_t pixel_data_size = padded_row_size * height;
      const uint32_t file_size = 54 + pixel_data_size;

      std::ofstream file(bmp_filename, std::ios::binary);
      if (!file.is_open())
      {
        std::cerr << "Cannot create file: " << bmp_filename << std::endl;
        return false;
      }

      file.write("BM", 2);
      file.write(reinterpret_cast<const char *>(&file_size), 4);
      file.write("\x00\x00\x00\x00", 4);
      const uint32_t pixel_data_offset = 54;
      file.write(reinterpret_cast<const char *>(&pixel_data_offset), 4);

      const uint32_t header_size = 40;
      const uint16_t planes = 1;
      const uint16_t bits_per_pixel = 24;
      const uint32_t compression = 0;

      file.write(reinterpret_cast<const char *>(&header_size), 4);
      file.write(reinterpret_cast<const char *>(&width), 4);
      file.write(reinterpret_cast<const char *>(&height), 4);
      file.write(reinterpret_cast<const char *>(&planes), 2);
      file.write(reinterpret_cast<const char *>(&bits_per_pixel), 2);
      file.write(reinterpret_cast<const char *>(&compression), 4);
      file.write(reinterpret_cast<const char *>(&pixel_data_size), 4);
      file.write("\x13\x0B\x00\x00", 4);
      file.write("\x13\x0B\x00\x00", 4);
      file.write("\x00\x00\x00\x00", 4);
      file.write("\x00\x00\x00\x00", 4);

      std::vector<unsigned char> row_buffer(padded_row_size, 0);

      for (int y = height - 1; y >= 0; --y)
      {

        for (uint32_t x = 0; x < width; ++x)
        {
          size_t src_idx = image.getPixelIndex(x, y);
          size_t dst_idx = x * 3;

          row_buffer[dst_idx] = image.data[src_idx + 2];
          row_buffer[dst_idx + 1] = image.data[src_idx + 1];
          row_buffer[dst_idx + 2] = image.data[src_idx];
        }

        file.write(reinterpret_cast<const char *>(row_buffer.data()), padded_row_size);
      }

      if (!file.good())
      {
        std::cerr << "Failed to write BMP file: " << bmp_filename << std::endl;
        return false;
      }

      std::cout << "Image saved (ultra-fast BMP): " << bmp_filename << std::endl;
      return true;
    }

  }
}
