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

#include <cstdint>
#include <istream>
#include <ostream>
#include <type_traits>


namespace rigel::base
{

namespace detail
{

template <typename T, typename = void>
struct HasSize : std::false_type
{
};


template <typename T>
struct HasSize<T, std::void_t<decltype(std::declval<T>().size())>>
  : std::true_type
{
};


template <typename T, typename = void>
struct HasData : std::false_type
{
};


template <typename T>
struct HasData<T, std::void_t<decltype(std::declval<T>().data())>>
  : std::true_type
{
};


template <typename T, typename = void>
struct HasEmpty : std::false_type
{
};


template <typename T>
struct HasEmpty<T, std::void_t<decltype(std::declval<T>().empty())>>
  : std::true_type
{
};


template <typename T>
constexpr auto IsContainerV = (HasSize<T>::value) && (HasData<T>::value) &&
  (HasEmpty<T>::value);

} // namespace detail


template <typename T>
void writeArray(std::ostream& stream, const T* pData, const size_t count)
{
  static_assert(std::is_trivially_copyable_v<T>);
  stream.write(reinterpret_cast<const char*>(pData), sizeof(T) * count);
}


template <typename T>
std::enable_if_t<!detail::IsContainerV<T>>
  write(std::ostream& stream, const T& value)
{
  writeArray(stream, &value, 1);
}


template <typename Container>
std::enable_if_t<detail::IsContainerV<Container>>
  write(std::ostream& stream, const Container& data)
{
  write(stream, static_cast<uint16_t>(data.size()));

  if (!data.empty())
  {
    writeArray(stream, data.data(), data.size());
  }
}


template <typename T>
void readArray(std::istream& stream, T* pData, const size_t count)
{
  static_assert(std::is_trivially_copyable_v<T>);
  stream.read(reinterpret_cast<char*>(pData), sizeof(T) * count);
}


template <typename T>
std::enable_if_t<!detail::IsContainerV<T>> read(std::istream& stream, T& value)
{
  readArray(stream, &value, 1);
}


template <typename T>
T read(std::istream& stream)
{
  T result;
  read(stream, result);
  return result;
}


template <typename Container>
std::enable_if_t<detail::IsContainerV<Container>>
  read(std::istream& stream, Container& data)
{
  const auto size = read<uint16_t>(stream);
  data.resize(size);

  if (size > 0)
  {
    readArray(stream, data.data(), data.size());
  }
}

} // namespace rigel
