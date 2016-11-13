#include <algorithm>
#include <sdl_input.hxx>

namespace proto
{
	auto sdl_input::key_down(key_type p_key) const
		-> bool
	{
		return m_KeyDown.at(p_key);
	}
	
	auto sdl_input::reset_keys()
		-> void
	{
		for(size_t t_ix = 0; t_ix < m_KeyDown.size(); ++t_ix)
		{
			if(!m_KeyRepeat[t_ix])
				m_KeyDown[t_ix] = false;
		}
	}
	
	auto sdl_input::proc_modifiers(mod_type p_mod, bool p_isRepeat)
		-> void
	{
		#define chkMod(__mod, __key) if(p_mod&(__mod)){m_KeyDown.at(__key)=true; if(p_isRepeat) {m_KeyRepeat.at(__key)=true; }}
		chkMod(KMOD_LALT, SDL_SCANCODE_LALT)
		chkMod(KMOD_LSHIFT, SDL_SCANCODE_LSHIFT)		
	}
	
	auto sdl_input::process_events()
		-> void
	{
		// Reset all keys that are not in repeat mode
		reset_keys();
		
		// Poll all buffered events
		SDL_Event t_event;
		
		while(SDL_PollEvent(&t_event))
		{
			switch(t_event.type)
			{
				case SDL_QUIT:
				{
					std::exit(EXIT_SUCCESS);
					break;
				}
				
				// Key was released
				case SDL_KEYUP:
				{
					m_KeyDown[t_event.key.keysym.scancode] = false;
					m_KeyRepeat[t_event.key.keysym.scancode] = false;

					break;
				}

				// Key was pressed
				case SDL_KEYDOWN:
				{
					if(t_event.key.repeat) m_KeyRepeat[t_event.key.keysym.scancode] = true;

					m_KeyDown[t_event.key.keysym.scancode] = true;

					proc_modifiers(t_event.key.keysym.mod, t_event.key.repeat);

					break;
				}
				
				// Disregard all other events
				default:
					break;
			}
		}
	}
}