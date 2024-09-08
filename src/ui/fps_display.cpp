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

#include "ui/fps_display.hpp"

#include "base/math_utils.hpp"

#include <imgui.h>
#include <imgui_internal.h>

#include <iomanip>
#include <sstream>
#include <string>


namespace rigel::ui
{

namespace
{

const auto PRE_FILTER_WEIGHT = 0.7f;
const auto FILTER_WEIGHT = 0.9f;

} // namespace


FpsDisplay::FpsDisplay(base::Vec2f position)
  : mPosition(position)
{
}


void FpsDisplay::updateAndRender(const double totalElapsed)
{
  updateFilteredFrameTime(totalElapsed);

  const auto smoothedFps = base::round(1.0f / mFilteredFrameTime);

  std::stringstream statsReport;
  // clang-format off
  statsReport
    << smoothedFps << " FPS, "
    << std::setw(4) << std::fixed << std::setprecision(2)
    << totalElapsed * 1000.0 << " ms";
  // clang-format on

  displayText(statsReport.str());
}


void FpsDisplay::updateAndRender(
  const double totalElapsed,
  const double elapsedCpu,
  const double elapsedGpu)
{
#ifdef RIGEL_HAVE_BOOST
  mFrameTimesHistory.push_back(float(totalElapsed));
#endif

  updateFilteredFrameTime(totalElapsed);

  const auto smoothedFps = base::round(1.0f / mFilteredFrameTime);

  std::stringstream statsReport;
  // clang-format off
  statsReport
    << smoothedFps << " FPS, "
    << std::setw(4) << std::fixed << std::setprecision(2)
    << totalElapsed * 1000.0 << " ms, "
    << elapsedCpu * 1000.0 << " ms (CPU), "
    << elapsedGpu * 1000.0 << " ms (GPU)";
  // clang-format on

  const auto reportString = statsReport.str();
  displayText(reportString);

#ifdef RIGEL_HAVE_BOOST
  // Draw frame-time graph
  if (mFrameTimesHistory.size() < 2)
  {
    return;
  }

  auto pFont = ImGui::GetFont();
  const auto textSize = pFont->CalcTextSizeA(
    ImGui::GetDrawListSharedData()->FontSize,
    FLT_MAX,
    -1.0f,
    reportString.data(),
    reportString.data() + reportString.size());

  auto pDrawList = ImGui::GetForegroundDrawList();
  const auto color = IM_COL32(255, 255, 255, 255);

  const auto basePos = ImVec2{textSize.x + 20 + mPosition.x, mPosition.y};

  // Average frame time in ms
  const auto average = mFilteredFrameTime * 1000.0f;

  for (auto i = 0; i < int(mFrameTimesHistory.size() - 1); i++)
  {
    // Convert to ms, this gives us one pixel per ms
    const auto v0 = mFrameTimesHistory[i] * 1000.0f;
    const auto v1 = mFrameTimesHistory[i + 1] * 1000.0f;

    const auto pos0 = -(v0 - average) + textSize.y / 2.0f;
    const auto pos1 = -(v1 - average) + textSize.y / 2.0f;

    pDrawList->AddLine(
      ImVec2{basePos.x + i, basePos.y + pos0},
      ImVec2{basePos.x + i + 1, basePos.y + pos1},
      color);
  }
#endif
}


void FpsDisplay::updateFilteredFrameTime(const double totalElapsed)
{
  mPreFilteredFrameTime = base::lerp(
    static_cast<float>(totalElapsed), mPreFilteredFrameTime, PRE_FILTER_WEIGHT);
  mFilteredFrameTime =
    base::lerp(mPreFilteredFrameTime, mFilteredFrameTime, FILTER_WEIGHT);
}


void FpsDisplay::displayText(const std::string& text)
{
  auto pDrawList = ImGui::GetForegroundDrawList();

  const auto color = IM_COL32(255, 255, 255, 255);

  pDrawList->AddText(
    {mPosition.x, mPosition.y}, color, text.data(), text.data() + text.size());
}

} // namespace rigel::ui
