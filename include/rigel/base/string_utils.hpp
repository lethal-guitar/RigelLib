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

#include <string>
#include <string_view>
#include <vector>

namespace rigel::strings
{

/** Split a string based on a single char delimiter and return the output
 */
[[nodiscard]] std::vector<std::string>
  split(std::string_view input, char delimiter);

/** Checks if an input string has the given prefix
 */
[[nodiscard]] bool
  startsWith(std::string_view input, std::string_view prefix) noexcept;

/** Removes all occurences of 'what' from the left of the input, until any
 * non-'what' is found
 *
 * Does everything inplace.
 */
std::string&
  trimLeft(std::string& input, const char* what = "\n\r\t ") noexcept;

/** Like trimLeft, but works for temporaries/r-values (returns a new value) */
[[nodiscard]] std::string
  trimLeft(std::string_view input, const char* what = "\n\r\t ");

/** Removes all occurences of 'what' from the right of the input, until any
 * non-'what' is found
 *
 * Does everything inplace.
 */
std::string&
  trimRight(std::string& input, const char* what = "\n\r\t ") noexcept;

/** Like trimRight, but works for temporaries/r-values (returns a new value) */
[[nodiscard]] std::string
  trimRight(std::string_view input, const char* what = "\n\r\t ");

/** Removes all occurences of 'what' from the right and left of the input, until
 * any non-'what' is found
 *
 * Does everything inplace.
 */
std::string& trim(std::string& input, const char* what = "\n\r\t ") noexcept;

/** Like trim, but works for temporaries/r-values (returns a new value) */
[[nodiscard]] std::string
  trim(std::string_view input, const char* what = "\n\r\t ");

[[nodiscard]] std::string toUppercase(std::string_view input);

[[nodiscard]] std::string toLowercase(std::string_view input);

size_t utf8len(const std::string& input);
size_t utf8lenToBytes(const std::string& input, size_t utf8len);

} // namespace rigel::strings
