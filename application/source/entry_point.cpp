#include <dial/entry_point.hpp>
#include <SDL3/SDL.h>
#include <imgui.h>

DIAL_ENTRY_POINT
(	[]
	{	SDL_SetWindowSize(SDL_GetWindows(NULL)[0], 750, 750);
		ImGui::GetIO().IniFilename = NULL;
		return true;
	}
)