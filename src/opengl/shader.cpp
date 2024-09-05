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

#include "opengl/shader.hpp"

#include "opengl/utils.hpp"

#include <memory>
#include <numeric>
#include <stdexcept>
#include <string>


namespace rigel::opengl
{

namespace
{

#ifdef RIGEL_USE_GL_ES

const auto SHADER_PREAMBLE = R"shd(
#version 100

#define ATTRIBUTE attribute
#define OUT varying
#define IN varying
#define TEXTURE_LOOKUP texture2D
#define OUTPUT_COLOR gl_FragColor
#define OUTPUT_COLOR_DECLARATION
#define SET_POINT_SIZE(size) gl_PointSize = size;
#define HIGHP highp
#define DEFAULT_PRECISION_DECLARATION precision mediump float;
)shd";

#else

  // We generally want to stick to GLSL version 130 (from OpenGL 3.0) in order
  // to maximize compatibility with older graphics cards. Unfortunately, Mac OS
  // only supports GLSL 150 (from OpenGL 3.2), even when requesting a OpenGL 3.0
  // context. Therefore, we use different GLSL versions depending on the
  // platform.
  #if defined(__APPLE__)
const auto SHADER_PREAMBLE = R"shd(
#version 150

#define ATTRIBUTE in
#define OUT out
#define IN in
#define TEXTURE_LOOKUP texture
#define OUTPUT_COLOR outputColor
#define OUTPUT_COLOR_DECLARATION out vec4 outputColor;
#define SET_POINT_SIZE
#define HIGHP
#define DEFAULT_PRECISION_DECLARATION
)shd";
  #else
const auto SHADER_PREAMBLE = R"shd(
#version 130

#define ATTRIBUTE in
#define OUT out
#define IN in
#define TEXTURE_LOOKUP texture2D
#define OUTPUT_COLOR outputColor
#define OUTPUT_COLOR_DECLARATION out vec4 outputColor;
#define SET_POINT_SIZE
#define HIGHP
#define DEFAULT_PRECISION_DECLARATION
)shd";
  #endif

#endif


Handle<tag::Shader> compileShader(const std::string& source, GLenum type)
{
  auto shader = Handle<tag::Shader>::create(type);

  const auto sourcePtr = source.c_str();
  glShaderSource(shader.mHandle, 1, &sourcePtr, nullptr);
  glCompileShader(shader.mHandle);

  GLint compileStatus = 0;
  glGetShaderiv(shader.mHandle, GL_COMPILE_STATUS, &compileStatus);

  if (!compileStatus)
  {
    GLint infoLogSize = 0;
    glGetShaderiv(shader.mHandle, GL_INFO_LOG_LENGTH, &infoLogSize);

    if (infoLogSize > 0)
    {
      std::unique_ptr<char[]> infoLogBuffer(new char[infoLogSize]);
      glGetShaderInfoLog(
        shader.mHandle, infoLogSize, nullptr, infoLogBuffer.get());

      throw std::runtime_error(
        std::string{"Shader compilation failed:\n\n"} + infoLogBuffer.get());
    }
    else
    {
      throw std::runtime_error(
        "Shader compilation failed, but could not get info log");
    }
  }

  return shader;
}


auto useTemporarily(const GLuint shaderHandle)
{
  GLint currentProgram;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
  glUseProgram(shaderHandle);

  return base::defer([currentProgram]() { glUseProgram(currentProgram); });
}

} // namespace


Shader::Shader(const ShaderSpec& spec)
  : mProgram(Handle<tag::Program>::create())
  , mAttributeSpecs(spec.mAttributes)
{
  auto vertexShader = compileShader(
    std::string{SHADER_PREAMBLE} + spec.mVertexSource, GL_VERTEX_SHADER);
  auto fragmentShader = compileShader(
    std::string{SHADER_PREAMBLE} + spec.mFragmentSource, GL_FRAGMENT_SHADER);

  glAttachShader(mProgram.mHandle, vertexShader.mHandle);
  glAttachShader(mProgram.mHandle, fragmentShader.mHandle);

  {
    auto index = 0;
    for (const auto& attribute : mAttributeSpecs)
    {
      glBindAttribLocation(mProgram.mHandle, index, attribute.mName);
      ++index;
    }
  }

  glLinkProgram(mProgram.mHandle);

  GLint linkStatus = 0;
  glGetProgramiv(mProgram.mHandle, GL_LINK_STATUS, &linkStatus);
  if (!linkStatus)
  {
    GLint infoLogSize = 0;
    glGetProgramiv(mProgram.mHandle, GL_INFO_LOG_LENGTH, &infoLogSize);

    if (infoLogSize > 0)
    {
      std::unique_ptr<char[]> infoLogBuffer(new char[infoLogSize]);
      glGetProgramInfoLog(
        mProgram.mHandle, infoLogSize, nullptr, infoLogBuffer.get());

      throw std::runtime_error(
        std::string{"Shader program linking failed:\n\n"} +
        infoLogBuffer.get());
    }
    else
    {
      throw std::runtime_error(
        "Shader program linking failed, but could not get info log");
    }
  }

  // Bind texture sampler names to texture units
  auto guard = useTemporarily(mProgram.mHandle);

  for (auto i = 0u; i < spec.mTextureUnitNames.size(); ++i)
  {
    setUniform(spec.mTextureUnitNames[i], int(i));
  }
}


void Shader::use() const
{
  glUseProgram(mProgram.mHandle);
}


GLint Shader::location(const std::string& name) const
{
  auto it = mLocationCache.find(name);
  if (it == mLocationCache.end())
  {
    const auto location = glGetUniformLocation(mProgram.mHandle, name.c_str());
    std::tie(it, std::ignore) = mLocationCache.emplace(name, location);
  }

  return it->second;
}


base::ScopeGuard useTemporarily(const Shader& shader)
{
  return useTemporarily(shader.handle());
}


void submitVertexAttributeSetup(base::ArrayView<AttributeSpec> attributes)
{
  const auto totalSize = std::accumulate(
    attributes.begin(),
    attributes.end(),
    0,
    [](int sum, const AttributeSpec& attribute) {
      return sum + static_cast<int>(attribute.mSize);
    });

  auto index = 0;
  auto offset = 0;
  for (const auto& attribute : attributes)
  {
    const auto size = static_cast<int>(attribute.mSize);
    glVertexAttribPointer(
      index,
      size,
      GL_FLOAT,
      GL_FALSE,
      sizeof(float) * totalSize,
      toAttribOffset(offset));

    offset += size * sizeof(float);
    ++index;
  }
}

} // namespace rigel::opengl
