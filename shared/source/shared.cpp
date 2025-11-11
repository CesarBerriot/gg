#include "shared.hpp"

#include <stdexcept>

#include "SDL3/SDL_video.h"

namespace shared
{	SDL_Window * get_first_sdl_window()
	{	SDL_Window ** windows = SDL_GetWindows(NULL);
		if(!windows)
			throw std::runtime_error("SDL_GetWindows failure.");
		SDL_Window * result = windows[0];
		SDL_free(windows);
		return result;
	}
}
