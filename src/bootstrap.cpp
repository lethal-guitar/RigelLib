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

#include "base/warnings.hpp"
#include "opengl/opengl.hpp"
#include "sdl_utils/error.hpp"
#include "sdl_utils/ptr.hpp"
#include "ui/imgui_integration.hpp"

RIGEL_DISABLE_WARNINGS
#include <loguru.hpp>
#include <lyra/help.hpp>
RIGEL_RESTORE_WARNINGS


#ifdef _WIN32

  #include <stdio.h>
  #include <win32_SetProcessDpiAware.h>
  #include <windows.h>


static std::optional<rigel::base::ScopeGuard> win32ReenableStdIo()
{
  if (AttachConsole(ATTACH_PARENT_PROCESS))
  {
    std::cin.clear();
    std::cout.flush();
    std::cerr.flush();

    FILE* fp = nullptr;
    freopen_s(&fp, "CONIN$", "r", stdin);
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);

    std::cout << std::endl;

    return rigel::base::defer([]() {
      std::cout.flush();
      std::cerr.flush();

      // This is a hack to make the console output behave like it does when
      // running a genuine console app (i.e. subsystem set to console).
      // The thing is that even though we attach to the console that has
      // launched us, the console itself is not actually waiting for our
      // process to terminate, since it treats us as a GUI application.
      // This means that we can write our stdout/stderr to the console, but
      // the console won't show a new prompt after our process has terminated
      // like it would do with a console application. This makes command line
      // usage awkward because users need to press enter once after each
      // invocation of RigelEngine in order to get a new prompt.
      // By sending a enter key press message to the parent console, we do this
      // automatically.
      SendMessageA(GetConsoleWindow(), WM_CHAR, VK_RETURN, 0);
      FreeConsole();
    });
  }

  return std::nullopt;
}

static void enableDpiAwareness()
{
  win32_SetProcessDpiAware();
}

#else

static std::optional<rigel::base::ScopeGuard> win32ReenableStdIo()
{
  return std::nullopt;
}


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

  const auto width = [&]() {
    if (config.fullscreen)
    {
      return displayMode.w;
    }

    if (config.windowWidth == -1)
    {
      return displayMode.w * 80 / 100;
    }

    return config.windowWidth;
  }();
  const auto height = [&]() {
    if (config.fullscreen)
    {
      return displayMode.h;
    }

    if (config.windowHeight == -1)
    {
      return displayMode.h * 80 / 100;
    }

    return config.windowHeight;
  }();

  LOG_F(
    INFO,
    "Creating window in %s mode, size: %dx%d",
    config.fullscreen ? "fullscreen" : "windowed",
    width,
    height);
  auto pWindow = sdl_utils::wrap(sdl_utils::check(SDL_CreateWindow(
    config.windowTitle.c_str(),
    config.windowX != -1 ? config.windowX : SDL_WINDOWPOS_CENTERED,
    config.windowY != -1 ? config.windowY : SDL_WINDOWPOS_CENTERED,
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


void loadGameControllerDbForOldSdl()
{
  // SDL versions before 2.0.10 didn't check the SDL_GAMECONTROLLERCONFIG_FILE
  // env var. To make working with game controllers more consistent across
  // SDL versions, we implement this ourselves in case the SDL version being
  // used is older.
  SDL_version version;
  SDL_GetVersion(&version);

  if (version.major == 2 && version.minor == 0 && version.patch < 10)
  {
    LOG_F(
      INFO,
      "SDL older than 2.0.10, manually checking "
      "SDL_GAMECONTROLLERCONFIG_FILE env var");
    if (const auto pMappingsFile = SDL_getenv("SDL_GAMECONTROLLERCONFIG_FILE"))
    {
      SDL_GameControllerAddMappingsFromFile(pMappingsFile);
    }
  }
}


void showErrorBox(const char* message)
{
  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message, nullptr);
}


void runAppUnguarded(
  const WindowConfig& config,
  std::function<void(SDL_Window*)> initFunc,
  std::function<bool(SDL_Window*)> runFrameFunc)
{
  using base::defer;

  sdl_utils::check(SDL_GL_LoadLibrary(nullptr));

  setGLAttributes(config);
  auto pWindow = createWindow(config);

  LOG_F(INFO, "Initializing OpenGL context");
  SDL_GLContext pGlContext =
    sdl_utils::check(SDL_GL_CreateContext(pWindow.get()));
  auto glGuard = defer([pGlContext]() { SDL_GL_DeleteContext(pGlContext); });

  LOG_F(INFO, "Loading OpenGL function pointers");
  opengl::loadGlFunctions();

  // On some platforms, an initial swap is necessary in order for the next
  // frame to show up on screen.
  SDL_GL_SetSwapInterval(config.enableVsync ? 1 : 0);
  SDL_GL_SwapWindow(pWindow.get());

  SDL_DisableScreenSaver();
  SDL_ShowCursor(SDL_DISABLE);

  LOG_F(INFO, "Initializing Dear ImGui");
  ui::imgui_integration::init(pWindow.get(), pGlContext, {});
  auto imGuiGuard = defer([]() { ui::imgui_integration::shutdown(); });

  initFunc(pWindow.get());

  while (runFrameFunc(pWindow.get()))
  {
    //
  }

  LOG_F(INFO, "Exiting");
}

} // namespace


[[nodiscard]] base::ScopeGuard initSdl()
{
  using base::defer;

  enableDpiAwareness();
  loadGameControllerDbForOldSdl();

  LOG_F(INFO, "Initializing SDL");
  sdl_utils::check(
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER));
  auto sdlGuard = defer([]() { SDL_Quit(); });

  LOG_F(
    INFO,
    "SDL backends: %s, %s",
    SDL_GetCurrentVideoDriver(),
    SDL_GetCurrentAudioDriver());

  return sdlGuard;
}


int runApp(
  const WindowConfig& config,
  std::function<bool(SDL_Window*)> runFrameFunc)
{
  return runApp(
    config, [](SDL_Window*) {}, std::move(runFrameFunc));
}


int runApp(
  const WindowConfig& config,
  std::function<void(SDL_Window*)> initFunc,
  std::function<bool(SDL_Window*)> runFrameFunc)
{
  try
  {
    std::optional<base::ScopeGuard> sdlGuard;

    if (!SDL_WasInit(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER))
    {
      sdlGuard.emplace(initSdl());
    }

    runAppUnguarded(config, std::move(initFunc), std::move(runFrameFunc));
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


std::optional<int> parseArgs(
  int argc,
  char** argv,
  std::function<void(lyra::cli&)> setupCliOptionsFunc,
  std::function<bool()> validateCliOptionsParseResultFunc)
{
  // On Windows, our executable is a GUI application (subsystem win32), which
  // means that it can't be used as a command-line application - stdout and
  // stdin are not connected to the terminal that launches the executable in
  // case of a GUI application.
  // However, it's possible to detect that we've been launched from a terminal,
  // and then manually attach our stdin/stdout to that terminal. This makes
  // our command line interface usable on Windows.
  // It's not perfect, because the terminal itself doesn't actually know
  // that a process it has launched has now attached to it, so it keeps happily
  // accepting user input, it doesn't wait for our process to terminate like
  // it normally does when running a console application. But since we don't
  // need interactive command line use, it's good enough for our case - we
  // can output some text to the terminal and then detach again.
  auto win32IoGuard = win32ReenableStdIo();

  auto showHelp = false;

  auto optionsParser = lyra::cli() | lyra::help(showHelp);

  // Add user-supplied options
  setupCliOptionsFunc(optionsParser);

  const auto parseResult = optionsParser.parse({argc, argv});

  if (showHelp)
  {
    std::cout << optionsParser << '\n';
    return 0;
  }

  if (!parseResult)
  {
    std::cerr << "ERROR: " << parseResult.message() << "\n\n";
    std::cerr << optionsParser << '\n';
    return -1;
  }

  // Run user-supplied post-parsing validation function
  if (!validateCliOptionsParseResultFunc())
  {
    return -1;
  }

  // Once we're done, detach from the console. See comment above
  win32IoGuard.reset();

  return std::nullopt;
}

} // namespace rigel
