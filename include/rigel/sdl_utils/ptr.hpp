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

#include <rigel/base/warnings.hpp>

RIGEL_DISABLE_WARNINGS
#include <SDL.h>

#ifdef RIGEL_HAVE_SDL_MIXER
  #include <SDL_mixer.h>
#endif

#ifdef RIGEL_HAVE_SDL_TTF
  #include <SDL_ttf.h>
#endif
RIGEL_RESTORE_WARNINGS

#include <memory>


namespace rigel::sdl_utils
{

namespace detail
{

struct Deleter
{
  void operator()(SDL_Window* ptr) { SDL_DestroyWindow(ptr); }
  void operator()(SDL_GameController* ptr) { SDL_GameControllerClose(ptr); }
  void operator()(SDL_Surface* ptr) { SDL_FreeSurface(ptr); }

#ifdef RIGEL_HAVE_SDL_MIXER
  void operator()(Mix_Chunk* ptr) { Mix_FreeChunk(ptr); }
  void operator()(Mix_Music* ptr) { Mix_FreeMusic(ptr); }
#endif

#ifdef RIGEL_HAVE_SDL_TTF
  void operator()(TTF_Font* ptr) { TTF_CloseFont(ptr); }
#endif
};

} // namespace detail


template <typename SDLType>
using Ptr = std::unique_ptr<SDLType, detail::Deleter>;


template <typename SDLType>
[[nodiscard]] auto wrap(SDLType* ptr)
{
  return Ptr<SDLType>{ptr};
}

} // namespace rigel::sdl_utils
