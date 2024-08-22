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
#include <imgui/imgui.h>
#include <lyra/lyra.hpp>


namespace
{

struct CommandLineOptions
{
  bool fullscreen = false;
  bool showImGuiWindow = true;
};


bool runOneFrame(SDL_Window* pWindow, const CommandLineOptions& opts)
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

    if (opts.showImGuiWindow)
    {
      ImGui::Button("ImGui Test");
    }
  }


  // Present frame
  SDL_GL_SwapWindow(pWindow);

  // Keep running
  return true;
}

} // namespace


int main(int argc, char** argv)
{
  rigel::WindowConfig windowConfig;
  windowConfig.windowTitle = "Hello";
  windowConfig.fullscreen = false;

  CommandLineOptions opts;


  rigel::runApp(
    argc,
    argv,
    [&opts](lyra::cli& argsParser) {
      // Configure Lyra CLI argument parser with command line arguments.
      // Writes the result into `opts`.

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
      // This allows doing additional verification and post-processing on the
      // parsed command line args

      windowConfig.fullscreen = opts.fullscreen;

      return true;
    },

    windowConfig,
    [&opts](SDL_Window* pWindow) {
      // This is invoked continuously until it returns false
      return runOneFrame(pWindow, opts);
    });
}
