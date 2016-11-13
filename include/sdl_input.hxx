// Temporary input manager that directly uses SDL.
// Obviously only works with SDL renderers.

#pragma once

#include <array>
#include <functional>
#include <cstdint>

// Pulls in SDL2 headers
#include <renderer_factory.hxx>


namespace proto
{
	class sdl_input
	{
		public:
			using key_type = SDL_Scancode;
			using mod_type = uint16_t;
		
		public:
			auto key_down(key_type) const
				-> bool;
				
			auto process_events()
				-> void;
				
		private:
			auto reset_keys()
				-> void;
			
			auto proc_modifiers(mod_type, bool)
				-> void;
		
		private:
			std::array<bool, 256> m_KeyDown{0};
			std::array<bool, 256> m_KeyRepeat{0};
	};
}