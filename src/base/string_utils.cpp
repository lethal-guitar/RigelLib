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

#include "string_utils.hpp"

#include <algorithm>
#include <cassert>
#include <cctype>

namespace rigel::strings
{

std::vector<std::string> split(std::string_view input, char delimiter)
{
  assert(
    static_cast<int>(delimiter) < 127 && "We only accept ASCII delimiters");

  std::vector<std::string> output;
  auto start = std::begin(input);
  auto end = std::end(input);
  decltype(start) next;
  while ((next = std::find(start, end, delimiter)) != end)
  {
    output.emplace_back(start, next);
    start = std::next(next, 1);
  }
  output.emplace_back(start, next);
  return output;
}

bool startsWith(std::string_view input, std::string_view prefix) noexcept
{
  return input.size() >= prefix.size() &&
    std::equal(std::begin(prefix), std::end(prefix), std::begin(input));
}

std::string& trimLeft(std::string& input, const char* what) noexcept
{
  input.erase(0, input.find_first_not_of(what));
  return input;
}

std::string trimLeft(std::string_view input, const char* what)
{
  std::string result{input};
  trimLeft(result, what);
  return result;
}

std::string& trimRight(std::string& input, const char* what) noexcept
{
  input.erase(input.find_last_not_of(what) + 1);
  return input;
}

std::string trimRight(std::string_view input, const char* what)
{
  std::string result{input};
  trimRight(result, what);
  return result;
}

std::string& trim(std::string& input, const char* what) noexcept
{
  trimLeft(trimRight(input, what), what);
  return input;
}

std::string trim(std::string_view input, const char* what)
{
  std::string result{input};
  trim(result, what);
  return result;
}

std::string toUppercase(std::string_view input)
{
  std::string result{input};
  std::transform(
    result.begin(), result.end(), result.begin(), [](const auto ch) {
      return static_cast<char>(std::toupper(ch));
    });
  return result;
}

std::string toLowercase(std::string_view input)
{
  std::string result{input};
  std::transform(
    result.begin(), result.end(), result.begin(), [](const auto ch) {
      return static_cast<char>(std::tolower(ch));
    });
  return result;
}

size_t utf8len(const std::string& input)
{
  size_t len = 0;
  for (auto& ch : input)
    if ((ch & 0xC0) != 0x80)
      len++;
  return len;
}

size_t utf8lenToBytes(const std::string& input, size_t utf8len)
{
  size_t len = input.size();
  if ((utf8len < 1) || (len < 1))
    return 0;
  size_t i = 0;
  do
  {
    i++;
    while ((i < len) && ((input[i] & 0xC0) == 0x80))
      i++;
    utf8len--;
  } while ((i < len) && (utf8len > 0));
  return i;
}

} // namespace rigel::strings
