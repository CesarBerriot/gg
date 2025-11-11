#include <dial/entry_point.hpp>
#include <SDL3/SDL.h>
#include <imgui.h>
#include "shared.hpp"

DIAL_ENTRY_POINT
(	[]
	{	ImGui::GetIO().IniFilename = NULL;
		SDL_Window * window = shared::get_first_sdl_window();
		return
			SDL_SetWindowResizable(window, false) &&
			SDL_SetWindowSize(window, 225, 55);
	}
)
