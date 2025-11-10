#include <dial/entry_point.hpp>
#include <SDL3/SDL.h>
#include <imgui.h>

DIAL_ENTRY_POINT
(	[]
	{	ImGui::GetIO().IniFilename = NULL;
		SDL_Window * window = SDL_GetWindows(NULL)[0];
		return
			SDL_SetWindowResizable(window, false) &&
			SDL_SetWindowSize(window, 225, 55);
	}
)