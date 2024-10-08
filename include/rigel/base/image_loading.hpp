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

#include <rigel/base/array_view.hpp>
#include <rigel/base/image.hpp>

#include <filesystem>
#include <optional>
#include <string>

namespace rigel::base
{

/** Load image from file (png, jpeg, other common formats - uses STB Image) */
std::optional<Image> loadImage(const std::filesystem::path& path);
std::optional<Image> loadImage(base::ArrayView<std::uint8_t> data);

Image loadImageOrThrow(const std::filesystem::path& path);

bool savePng(const std::filesystem::path& path, const Image& image);

} // namespace rigel::base
