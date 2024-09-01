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

#include "ui/imgui_integration.hpp"

RIGEL_DISABLE_WARNINGS
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>
RIGEL_RESTORE_WARNINGS

#include <algorithm>


namespace rigel::ui::imgui_integration
{

namespace
{

std::string gIniFilePath;

bool shouldConsumeEvent(const SDL_Event& event)
{
  const auto& io = ImGui::GetIO();

  switch (event.type)
  {
    case SDL_MOUSEWHEEL:
    case SDL_MOUSEBUTTONDOWN:
      return io.WantCaptureMouse;

    case SDL_TEXTINPUT:
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      return io.WantCaptureKeyboard;
  }

  return false;
}

} // namespace


void init(
  SDL_Window* pWindow,
  void* pGlContext,
  const std::optional<std::filesystem::path>& preferencesPath)
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

  ImGui_ImplSDL2_InitForOpenGL(pWindow, pGlContext);
  ImGui_ImplSDL2_SetGamepadMode(ImGui_ImplSDL2_GamepadMode_AutoAll);

  // Dear ImGui can figure out the correct GLSL version by itself. This handles
  // GL ES as well as regular GL.
  ImGui_ImplOpenGL3_Init(nullptr);

  if (preferencesPath)
  {
    const auto iniFilePath = *preferencesPath / "ImGui.ini";
    gIniFilePath = iniFilePath.u8string();
    ImGui::GetIO().IniFilename = gIniFilePath.c_str();
  }
  else
  {
    ImGui::GetIO().IniFilename = nullptr;
  }
}


void shutdown()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  gIniFilePath.clear();
}


bool handleEvent(const SDL_Event& event)
{
  const auto handledEvent = ImGui_ImplSDL2_ProcessEvent(&event);

  return handledEvent && shouldConsumeEvent(event);
}


void beginFrame(SDL_Window* pWindow)
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
}


void endFrame()
{
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

} // namespace rigel::ui::imgui_integration
