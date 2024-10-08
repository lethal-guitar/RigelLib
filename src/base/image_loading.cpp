/* Copyright (C) 2024, Nikolai Wuttke. All rights reserved.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "base/image_loading.hpp"

#include "base/byte_buffer.hpp"
#include "base/warnings.hpp"

RIGEL_DISABLE_WARNINGS
#include <stb_image.h>
#include <stb_image_write.h>
RIGEL_RESTORE_WARNINGS

#include <fstream>
#include <memory>


namespace rigel::base
{

namespace
{

std::optional<Image>
  convertToImage(unsigned char* pImageData, const int width, const int height)
{
  if (!pImageData)
  {
    return {};
  }

  const auto pPixels = reinterpret_cast<const Pixel*>(pImageData);
  const auto numPixels = static_cast<size_t>(width * height);
  PixelBuffer buffer{pPixels, pPixels + numPixels};
  return Image{
    std::move(buffer), static_cast<size_t>(width), static_cast<size_t>(height)};
}


void writeToFile(void* pContext, void* pData, int size)
{
  auto& file = *static_cast<std::ofstream*>(pContext);
  file.write(reinterpret_cast<const char*>(pData), size);
}

} // namespace


std::optional<Image> loadImage(const std::filesystem::path& path)
{
  if (const auto data = base::tryLoadFile(path))
  {
    return loadImage(*data);
  }

  return {};
}


std::optional<Image> loadImage(base::ArrayView<std::uint8_t> data)
{
  int width = 0;
  int height = 0;
  const auto imageDeleter = [](unsigned char* p) {
    stbi_image_free(p);
  };
  std::unique_ptr<unsigned char, decltype(imageDeleter)> pImageData{
    stbi_load_from_memory(
      data.data(), data.size(), &width, &height, nullptr, 4),
    imageDeleter};

  return convertToImage(pImageData.get(), width, height);
}


Image loadImageOrThrow(const std::filesystem::path& path)
{
  if (auto image = loadImage(path))
  {
    return *image;
  }

  throw std::runtime_error("Failed to load: " + path.u8string());
}


bool savePng(const std::filesystem::path& path, const Image& image)
{
  std::ofstream file(path, std::ios::binary);
  if (!file.is_open())
  {
    return false;
  }

  const auto width = static_cast<int>(image.width());
  const auto height = static_cast<int>(image.height());
  const auto result = stbi_write_png_to_func(
    writeToFile, &file, width, height, 4, image.pixelData().data(), width * 4);
  return result != 0;
}

} // namespace rigel::base
