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
#include <rigel/render/opengl.hpp>
#include <rigel/ui/imgui_integration.hpp>

#include <SDL_main.h>
#include <imgui/imgui.h>


int main(int argc, char** argv)
{
  rigel::WindowConfig windowConfig;
  windowConfig.windowTitle = "Hello";
  windowConfig.fullscreen = false;

  rigel::runApp(windowConfig, [](SDL_Window* pWindow) {
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

      ImGui::Button("ImGui Test");
    }


    // Present frame
    SDL_GL_SwapWindow(pWindow);

    // Keep running
    return true;
  });
}
