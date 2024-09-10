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

#include <rigel/opengl/opengl.hpp>

#include <type_traits>


namespace rigel::opengl
{

namespace tag
{

struct Buffer
{
};

struct Texture
{
};

struct Shader
{
};

struct Program
{
};

} // namespace tag


namespace detail
{

template <typename HandleType>
GLuint createHandle()
{
  namespace t = tag;

  static_assert(
    std::is_same_v<HandleType, t::Buffer> ||
    std::is_same_v<HandleType, t::Texture> ||
    std::is_same_v<HandleType, t::Program>);

  GLuint handle = 0;

  if constexpr (std::is_same_v<HandleType, t::Buffer>)
  {
    glGenBuffers(1, &handle);
  }
  else if constexpr (std::is_same_v<HandleType, t::Texture>)
  {
    glGenTextures(1, &handle);
  }
  else if constexpr (std::is_same_v<HandleType, t::Program>)
  {
    handle = glCreateProgram();
  }

  return handle;
}


inline GLuint createHandle(GLenum type)
{
  return glCreateShader(type);
}


template <typename HandleType>
void destroyHandle(const GLuint handle)
{
  namespace t = tag;

  static_assert(
    std::is_same_v<HandleType, t::Buffer> ||
    std::is_same_v<HandleType, t::Texture> ||
    std::is_same_v<HandleType, t::Shader> ||
    std::is_same_v<HandleType, t::Program>);

  if constexpr (std::is_same_v<HandleType, t::Buffer>)
  {
    glDeleteBuffers(1, &handle);
  }
  else if constexpr (std::is_same_v<HandleType, t::Texture>)
  {
    glDeleteTextures(1, &handle);
  }
  else if constexpr (std::is_same_v<HandleType, t::Shader>)
  {
    glDeleteShader(handle);
  }
  else if constexpr (std::is_same_v<HandleType, t::Program>)
  {
    glDeleteProgram(handle);
  }
}

} // namespace detail


template <typename HandleType>
class HandleBase
{
public:
  HandleBase() = default;

  HandleBase(const HandleBase&) = delete;
  HandleBase(HandleBase&& other)
    : mHandle(other.mHandle)
  {
    other.mHandle = 0;
  }

  ~HandleBase()
  {
    if (mHandle == 0)
    {
      return;
    }

    detail::destroyHandle<HandleType>(mHandle);
  }

  HandleBase& operator=(const HandleBase&) = delete;
  HandleBase& operator=(HandleBase&& other)
  {
    mHandle = std::exchange(other.mHandle, 0);
    return *this;
  }

  operator GLuint() const { return mHandle; }

  GLuint mHandle = 0;

protected:
  explicit HandleBase(GLuint handle)
    : mHandle(handle)
  {
  }
};


template <typename HandleType>
class Handle : public HandleBase<HandleType>
{
public:
  using HandleBase<HandleType>::HandleBase;

  static Handle<HandleType> create()
  {
    return Handle<HandleType>(detail::createHandle<HandleType>());
  }
};


template <>
class Handle<tag::Shader> : public HandleBase<tag::Shader>
{
public:
  using HandleBase::HandleBase;

  static Handle<tag::Shader> create(const GLenum type)
  {
    return Handle<tag::Shader>(detail::createHandle(type));
  }
};

} // namespace rigel::opengl
