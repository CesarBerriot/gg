#include <dial/entry_point.hpp>
#include <SDL3/SDL.h>
#include <imgui.h>
#include "shared.hpp"

DIAL_ENTRY_POINT
(	[]
	{	SDL_SetWindowSize(shared::get_first_sdl_window(), 750, 750);
		ImGui::GetIO().IniFilename = NULL;
		return true;
	}
)
