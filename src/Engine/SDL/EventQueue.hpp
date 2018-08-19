#pragma once

#include <deque>
#include <SDL.h>

#include <Engine/Extensions/Callable.hpp>

namespace isc
{
    namespace sdl
    {
        namespace EventQueue
        {
            static inline bool push(SDL_Event&& event)
            {
                return SDL_PushEvent(&event) == 1;
            }

            static inline bool poll(SDL_Event& event)
            {
                return SDL_PollEvent(&event) == 1;
            }

            template<typename TPredicate>
            static bool any(
                const Callable<TPredicate, bool(const SDL_Event&)>& predicate)
            {
                bool result = false;

                std::deque<SDL_Event> events;
                SDL_Event currentEvent;

                while (poll(currentEvent))
                {
                    result |= predicate(currentEvent);
                    events.emplace_back(std::move(currentEvent));
                }

                for (auto& event : events)
                {
                    push(std::move(event));
                }

                events.clear(); // events were moved

                return result;
            }
        }
    }
}
