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

#include <rigel/base/spatial_types.hpp>

#ifdef RIGEL_HAVE_BOOST
  #include <boost/circular_buffer.hpp>
#endif

#include <string>


namespace rigel::ui
{

class FpsDisplay
{
public:
  explicit FpsDisplay(base::Vec2f position = {});

  void updateAndRender(double totalElapsed);
  void
    updateAndRender(double totalElapsed, double elapsedCpu, double elapsedGpu);

private:
  void updateFilteredFrameTime(double totalElapsed);
  void displayText(const std::string& text);

#ifdef RIGEL_HAVE_BOOST
  boost::circular_buffer<float> mFrameTimesHistory{120};
#endif
  float mPreFilteredFrameTime = 0.0f;
  float mFilteredFrameTime = 0.0f;

  base::Vec2f mPosition;
};

} // namespace rigel::ui
