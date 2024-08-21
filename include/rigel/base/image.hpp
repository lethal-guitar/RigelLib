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

#pragma once

#include <rigel/base/color.hpp>

#include <cstdint>
#include <vector>


namespace rigel::base
{

using Pixel = rigel::base::Color;
using PixelBuffer = std::vector<Pixel>;


/** 2D image (bitmap), RGBA */
class Image
{
public:
  Image(PixelBuffer&& pixels, std::size_t width, std::size_t height);
  Image(const PixelBuffer& pixels, std::size_t width, std::size_t height);
  Image(std::size_t width, std::size_t height);

  const PixelBuffer& pixelData() const { return mPixels; }

  std::size_t width() const { return mWidth; }

  std::size_t height() const { return mHeight; }

  /** Returns a vertically flipped copy of the image */
  Image flipped() const;

  /** Returns a copy of the image, converted to premultiplied alpha */
  Image withPremultipliedAlpha() const;

  /** Copy given image's pixels into this image
   *
   * The source image must fit into the target image.
   */
  void insertImage(std::size_t x, std::size_t y, const Image& image);

  void insertImage(
    std::size_t x,
    std::size_t y,
    const PixelBuffer& pixels,
    std::size_t sourceWidth);

  /** Create Image containing only the pixels in the specified region */
  Image extractSubImage(
    std::size_t x,
    std::size_t y,
    std::size_t width,
    std::size_t height) const;

private:
  PixelBuffer mPixels;
  std::size_t mWidth;
  std::size_t mHeight;
};


} // namespace rigel::base
