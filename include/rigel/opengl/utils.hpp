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
#include <rigel/base/warnings.hpp>
#include <rigel/opengl/handle.hpp>

RIGEL_DISABLE_WARNINGS
#include <glm/vec4.hpp>
RIGEL_RESTORE_WARNINGS

namespace rigel::base
{

class Image;

}


namespace rigel::opengl
{

inline glm::vec4 toGlColor(const base::Color& color)
{
  return glm::vec4{color.r, color.g, color.b, color.a} / 255.0f;
}


inline void* toAttribOffset(std::uintptr_t offset)
{
  return reinterpret_cast<void*>(offset);
}


opengl::Handle<opengl::tag::Texture> createTexture(const base::Image& image);


class DummyVao
{
#ifndef RIGEL_USE_GL_ES
public:
  DummyVao()
  {
    glGenVertexArrays(1, &mVao);
    glBindVertexArray(mVao);
  }

  ~DummyVao() { glDeleteVertexArrays(1, &mVao); }

  DummyVao(const DummyVao&) = delete;
  DummyVao& operator=(const DummyVao&) = delete;

private:
  GLuint mVao;
#endif
};

} // namespace rigel::opengl
