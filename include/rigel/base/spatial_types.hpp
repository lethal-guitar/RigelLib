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

#include <rigel/base/math_utils.hpp>

#include <cstdint>
#include <tuple>


namespace rigel::base
{

template <typename ValueT>
struct Vec2T
{
  Vec2T() = default;
  Vec2T(const Vec2T&) = default;
  Vec2T(Vec2T&&) = default;
  constexpr Vec2T(const ValueT x_, const ValueT y_) noexcept
    : x(x_)
    , y(y_)
  {
  }
  Vec2T& operator=(const Vec2T&) = default;

  bool operator==(const Vec2T<ValueT>& rhs) const
  {
    return std::tie(x, y) == std::tie(rhs.x, rhs.y);
  }

  bool operator!=(const Vec2T<ValueT>& rhs) const { return !(*this == rhs); }


  ValueT x = 0;
  ValueT y = 0;
};


template <typename NewValueT, typename ValueT>
Vec2T<NewValueT> cast(const Vec2T<ValueT>& point)
{
  return {
    static_cast<NewValueT>(point.x),
    static_cast<NewValueT>(point.y),
  };
}


template <typename ValueT>
struct SizeT
{
  SizeT() = default;
  constexpr SizeT(const ValueT width_, const ValueT height_) noexcept
    : width(width_)
    , height(height_)
  {
  }

  bool operator==(const SizeT<ValueT>& rhs) const
  {
    return std::tie(width, height) == std::tie(rhs.width, rhs.height);
  }

  bool operator!=(const SizeT<ValueT>& rhs) const { return !(*this == rhs); }

  ValueT width = 0;
  ValueT height = 0;
};


template <typename NewValueT, typename ValueT>
SizeT<NewValueT> cast(const SizeT<ValueT>& size)
{
  return {
    static_cast<NewValueT>(size.width),
    static_cast<NewValueT>(size.height),
  };
}


template <typename ValueT>
struct Rect
{
  Vec2T<ValueT> topLeft;
  SizeT<ValueT> size;

  Vec2T<ValueT> bottomLeft() const
  {
    return Vec2T<ValueT>{
      topLeft.x, static_cast<ValueT>(topLeft.y + (size.height - 1))};
  }

  Vec2T<ValueT> bottomRight() const
  {
    return bottomLeft() + Vec2T<ValueT>{static_cast<ValueT>(size.width - 1), 0};
  }

  ValueT top() const { return topLeft.y; }

  ValueT bottom() const { return bottomLeft().y; }

  ValueT left() const { return topLeft.x; }

  ValueT right() const { return bottomRight().x; }

  bool intersects(const Rect& other) const
  {
    return left() <= other.right() && right() >= other.left() &&
      top() <= other.bottom() && bottom() >= other.top();
  }

  bool containsPoint(const Vec2T<ValueT>& point) const
  {
    return inRange(point.x, left(), right()) &&
      inRange(point.y, top(), bottom());
  }
};


template <typename ValueT>
Rect<ValueT> makeRect(Vec2T<ValueT> topLeft, Vec2T<ValueT> bottomRight)
{
  const auto sizeAsPoint = bottomRight - topLeft;
  return Rect<ValueT>{topLeft, SizeT<ValueT>{sizeAsPoint.x, sizeAsPoint.y}};
}


template <typename ValueT>
Vec2T<ValueT> operator+(const Vec2T<ValueT>& lhs, const Vec2T<ValueT>& rhs)
{
  return Vec2T<ValueT>(lhs.x + rhs.x, lhs.y + rhs.y);
}


template <typename ValueT>
Vec2T<ValueT> operator-(const Vec2T<ValueT>& lhs, const Vec2T<ValueT>& rhs)
{
  return Vec2T<ValueT>(lhs.x - rhs.x, lhs.y - rhs.y);
}


template <typename ValueT, typename ScalarT>
auto operator*(const Vec2T<ValueT>& point, const ScalarT scalar)
{
  return Vec2T<decltype(point.x * scalar)>{point.x * scalar, point.y * scalar};
}


template <typename ValueT, typename ScalarT>
auto operator/(const Vec2T<ValueT>& point, const ScalarT scalar)
{
  return Vec2T<decltype(point.x / scalar)>{point.x / scalar, point.y / scalar};
}


template <typename ValueT>
Vec2T<ValueT>& operator+=(Vec2T<ValueT>& lhs, const Vec2T<ValueT>& rhs)
{
  auto newPoint = lhs + rhs;
  std::swap(lhs, newPoint);
  return lhs;
}


template <typename ValueT>
Vec2T<ValueT>& operator-=(Vec2T<ValueT>& lhs, const Vec2T<ValueT>& rhs)
{
  auto newPoint = lhs - rhs;
  std::swap(lhs, newPoint);
  return lhs;
}


template <typename ValueT>
SizeT<ValueT> operator+(const SizeT<ValueT>& lhs, const SizeT<ValueT>& rhs)
{
  return SizeT<ValueT>(lhs.width + rhs.width, lhs.height + rhs.height);
}


template <typename ValueT>
SizeT<ValueT> operator-(const SizeT<ValueT>& lhs, const SizeT<ValueT>& rhs)
{
  return SizeT<ValueT>(lhs.width - rhs.width, lhs.height - rhs.height);
}


template <typename ValueT, typename ScalarT>
auto operator*(const SizeT<ValueT>& size, const ScalarT scalar)
{
  return SizeT<decltype(size.width * scalar)>{
    size.width * scalar, size.height * scalar};
}


template <typename ValueT>
SizeT<ValueT>& operator+=(SizeT<ValueT>& lhs, const SizeT<ValueT>& rhs)
{
  auto newSize = lhs + rhs;
  std::swap(lhs, newSize);
  return lhs;
}


template <typename ValueT>
SizeT<ValueT>& operator-=(SizeT<ValueT>& lhs, const SizeT<ValueT>& rhs)
{
  auto newSize = lhs - rhs;
  std::swap(lhs, newSize);
  return lhs;
}


template <typename ValueT>
Rect<ValueT>
  operator+(const Rect<ValueT>& rect, const Vec2T<ValueT>& translation)
{
  return Rect<ValueT>{
    rect.topLeft + Vec2T<ValueT>{translation.x, translation.y}, rect.size};
}


template <typename ValueT>
bool operator==(const Rect<ValueT>& lhs, const Rect<ValueT>& rhs)
{
  return std::tie(lhs.topLeft, lhs.size) == std::tie(rhs.topLeft, rhs.size);
}


template <typename ValueT>
bool operator!=(const Rect<ValueT>& lhs, const Rect<ValueT>& rhs)
{
  return !(lhs == rhs);
}


template <typename T>
auto asVec(const base::SizeT<T>& size)
{
  return base::Vec2T<T>{size.width, size.height};
}


template <typename T>
auto asSize(const base::Vec2T<T>& vec)
{
  return base::SizeT<T>{vec.x, vec.y};
}


using Vec2 = Vec2T<int>;
using Vec2f = Vec2T<float>;
using Size = SizeT<int>;
using SizeF = SizeT<float>;

} // namespace rigel::base
