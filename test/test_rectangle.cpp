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

#include <base/spatial_types.hpp>
#include <base/warnings.hpp>

RIGEL_DISABLE_WARNINGS
#include <catch2/catch_test_macros.hpp>
RIGEL_RESTORE_WARNINGS


TEST_CASE("Rectangle intersection")
{
  using namespace rigel;

  auto rect = base::Rect<int>{{4, 5}, {3, 6}};

  CHECK(rect.intersects({{4, 6}, {1, 1}}));
  CHECK(rect.intersects({{3, 6}, {3, 1}}));
  CHECK(rect.intersects({{5, 7}, {8, 10}}));
  CHECK(rect.intersects({{6, 10}, {2, 2}}));

  CHECK(!rect.intersects({{3, 5}, {1, 2}}));
  CHECK(!rect.intersects({{7, 2}, {1, 2}}));
  CHECK(!rect.intersects({{4, 4}, {3, 1}}));
  CHECK(!rect.intersects({{4, 11}, {3, 1}}));
}
