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

#include <rigel/base/warnings.hpp>

RIGEL_DISABLE_WARNINGS
#include <SDL.h>
#include <lyra/cli.hpp>
RIGEL_RESTORE_WARNINGS

#include <functional>
#include <optional>
#include <string>


namespace rigel
{

struct WindowConfig
{
  std::string windowTitle = "Rigel SDL Window";
  int windowWidth = 1920;
  int windowHeight = 1080;
  bool fullscreen = true;
  bool enableVsync = true;
  std::optional<uint8_t> depthBufferBits;
};


/** Init SDL+Gl, create window and run provided function in a loop
 *
 * This function initializes SDL, OpenGL, and Dear ImGui, then creates a window
 * using the specified configuration, and calls the provided function in a loop
 * until it returns false.
 *
 * Exceptions are caught and shown as message box before terminating the loop.
 *
 * The return value is the application exit code, to be returned from main().
 */
int runApp(
  const WindowConfig& config,
  std::function<bool(SDL_Window*)> runFrameFunc);

/** Same as runApp(), but with an additional init step */
int runApp(
  const WindowConfig& config,
  std::function<void(SDL_Window*)> initFunc,
  std::function<bool(SDL_Window*)> runFrameFunc);

std::optional<int> parseArgs(
  int argc, char** argv,
  std::function<void(lyra::cli&)> setupCliOptionsFunc,
  std::function<bool()> validateCliOptionsParseResultFunc);

}
