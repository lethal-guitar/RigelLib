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

#include "bootstrap.hpp"

#include "base/defer.hpp"
#include "base/warnings.hpp"
#include "render/opengl.hpp"
#include "sdl_utils/error.hpp"
#include "sdl_utils/ptr.hpp"
#include "ui/imgui_integration.hpp"

RIGEL_DISABLE_WARNINGS
#include <loguru.hpp>
RIGEL_RESTORE_WARNINGS


#ifdef _WIN32

  #include <win32_SetProcessDpiAware.h>

static void enableDpiAwareness()
{
  win32_SetProcessDpiAware();
}

#else

static void enableDpiAwareness() { }

#endif


namespace rigel
{

namespace
{

#ifdef __APPLE__
constexpr auto FULLSCREEN_FLAG = SDL_WINDOW_FULLSCREEN;
#else
constexpr auto FULLSCREEN_FLAG = SDL_WINDOW_FULLSCREEN_DESKTOP;
#endif


void setGLAttributes(const WindowConfig& config)
{
#ifdef RIGEL_USE_GL_ES
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#else
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  if (config.depthBufferBits)
  {
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, *config.depthBufferBits);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 32 - *config.depthBufferBits);
  }
}


sdl_utils::Ptr<SDL_Window> createWindow(const WindowConfig& config)
{
  LOG_SCOPE_FUNCTION(INFO);

  LOG_F(INFO, "Querying current screen resolution");

  SDL_DisplayMode displayMode;
  sdl_utils::check(SDL_GetDesktopDisplayMode(0, &displayMode));

  LOG_F(INFO, "Screen resolution is %dx%d", displayMode.w, displayMode.h);

  // clang-format off
  const auto windowFlags =
    SDL_WINDOW_RESIZABLE |
    SDL_WINDOW_ALLOW_HIGHDPI |
    SDL_WINDOW_OPENGL |
    (config.fullscreen ? FULLSCREEN_FLAG : 0);
  // clang-format on

  const auto width = config.fullscreen ? displayMode.w : config.windowWidth;
  const auto height = config.fullscreen ? displayMode.h : config.windowHeight;

  LOG_F(
    INFO,
    "Creating window in %s mode, size: %dx%d",
    config.fullscreen ? "fullscreen" : "windowed",
    width,
    height);
  auto pWindow = sdl_utils::wrap(sdl_utils::check(SDL_CreateWindow(
    config.windowTitle.c_str(),
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    width,
    height,
    windowFlags)));

  // Setting a display mode is necessary to make sure that exclusive
  // full-screen mode keeps using the desktop resolution. Without this,
  // switching to exclusive full-screen mode from windowed mode would result in
  // a screen resolution matching the window's last size.
  sdl_utils::check(SDL_SetWindowDisplayMode(pWindow.get(), &displayMode));

  return pWindow;
}


void showErrorBox(const char* message)
{
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message, nullptr);
}


void runAppUnguarded(
  const WindowConfig& config,
  std::function<bool(SDL_Window*)> runFrameFunc)
{
  using base::defer;

  enableDpiAwareness();

  LOG_F(INFO, "Initializing SDL");
  sdl_utils::check(
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER));
  auto sdlGuard = defer([]() { SDL_Quit(); });

  sdl_utils::check(SDL_GL_LoadLibrary(nullptr));

  setGLAttributes(config);
  auto pWindow = createWindow(config);

  LOG_F(INFO, "Initializing OpenGL context");
  SDL_GLContext pGlContext =
    sdl_utils::check(SDL_GL_CreateContext(pWindow.get()));
  auto glGuard = defer([pGlContext]() { SDL_GL_DeleteContext(pGlContext); });

  LOG_F(INFO, "Loading OpenGL function pointers");
  render::loadGlFunctions();

  SDL_GL_SetSwapInterval(1);
  SDL_GL_SwapWindow(pWindow.get());

  LOG_F(INFO, "Initializing Dear ImGui");
  ui::imgui_integration::init(pWindow.get(), pGlContext, {});
  auto imGuiGuard = defer([]() { ui::imgui_integration::shutdown(); });

  while (runFrameFunc(pWindow.get()))
  {
    //
  }

  LOG_F(INFO, "Exiting");
}

} // namespace


int runApp(
  const WindowConfig& config,
  std::function<bool(SDL_Window*)> runFrameFunc)
{
  try
  {
    runAppUnguarded(config, std::move(runFrameFunc));
    return 0;
  }
  catch (const std::exception& ex)
  {
    LOG_F(ERROR, "%s", ex.what());
    showErrorBox(ex.what());
    return -2;
  }
  catch (...)
  {
    LOG_F(ERROR, "Unknown error");
    showErrorBox("Unknown error");
    return -3;
  }
}

} // namespace rigel
