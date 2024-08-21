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

#ifdef RIGEL_HAVE_BOOST
  #include <boost/circular_buffer.hpp>
#endif


namespace rigel::ui
{

class FpsDisplay
{
public:
  void
    updateAndRender(double totalElapsed, double elapsedCpu, double elapsedGpu);


private:
#ifdef RIGEL_HAVE_BOOST
  boost::circular_buffer<float> mFrameTimesHistory{120};
#endif
  float mPreFilteredFrameTime = 0.0f;
  float mFilteredFrameTime = 0.0f;
};

} // namespace rigel::ui
