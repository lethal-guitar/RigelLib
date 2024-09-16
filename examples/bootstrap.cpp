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

#include <rigel/base/defer.hpp>
#include <rigel/bootstrap.hpp>
#include <rigel/opengl/opengl.hpp>
#include <rigel/ui/imgui_integration.hpp>

#include <SDL_main.h>
#include <imgui.h>
#include <lyra/lyra.hpp>


namespace
{

struct CommandLineOptions
{
  bool fullscreen = false;
  bool showImGuiWindow = true;
};


class App
{
public:
  explicit App(const CommandLineOptions& opts)
    : mShowImGuiWindow(opts.showImGuiWindow)
  {
  }

  bool runOneFrame(SDL_Window* pWindow)
  {
    // Handle events
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      rigel::ui::imgui_integration::handleEvent(event);

      switch (event.type)
      {
        case SDL_QUIT:
          return false;
      }
    }


    {
      // Dear ImGui integration
      rigel::ui::imgui_integration::beginFrame(pWindow);
      auto imGuiFrameGuard =
        rigel::base::defer([]() { rigel::ui::imgui_integration::endFrame(); });


      // OpenGL rendering, game/app logic etc. goes here
      glClearColor(0.6f, 0.85f, 0.9f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      if (mShowImGuiWindow)
      {
        ImGui::Button("ImGui Test");
      }
    }


    // Present frame
    SDL_GL_SwapWindow(pWindow);

    // Keep running
    return true;
  }

private:
  bool mShowImGuiWindow;
};

} // namespace


int main(int argc, char** argv)
{
  CommandLineOptions opts;

  const auto maybeErrorCode = rigel::parseArgs(
    argc,
    argv,
    [&opts](lyra::cli& argsParser) {
      // Configure Lyra CLI argument parser with command line arguments.
      // See Lyra library documenation for more details.

      // Here we write the result into `opts`.
      argsParser |= lyra::opt(opts.fullscreen)["-f"]["--fullscreen"].help(
        "Run in fullscreen mode");
      argsParser |= lyra::opt([&](const bool hide) {
        if (hide)
        {
          opts.showImGuiWindow = false;
        }
      })["--hide-imgui"];
    },
    [&]() {
      // This allows doing additional verification of the parsed command line
      // args. Return false to indicate an error
      return true;
    });

  if (maybeErrorCode)
  {
    return *maybeErrorCode;
  }

  rigel::WindowConfig windowConfig;
  windowConfig.windowTitle = "Hello";
  windowConfig.fullscreen = opts.fullscreen;

  std::unique_ptr<App> pApp;

  return rigel::runApp(
    windowConfig,
    [&pApp, &opts](SDL_Window*) {
      // Put initialization code here that should run right before the main
      // loop
      pApp = std::make_unique<App>(opts);
    },
    [&pApp](SDL_Window* pWindow) {
      // This is invoked continuously until it returns false
      return pApp->runOneFrame(pWindow);
    });
}
