#pragma once

#include "callable.hpp"

#include <deque>
#include <SDL.h>

namespace isc
{
    namespace sdl
    {
        namespace EventQueue
        {
            static inline bool push(SDL_Event&& event)
            {
                return to_bool(SDL_PushEvent(&event));
            }

            static inline bool poll(SDL_Event& event)
            {
                return to_bool(SDL_PollEvent(&event));
            }

            template<typename TPredicate>
            static bool any(
                const callable<TPredicate, bool(const SDL_Event&)>& predicate)
            {
                bool result = false;

                std::deque<SDL_Event> events;
                SDL_Event currentEvent;

                while (poll(currentEvent))
                {
                    result |= predicate(currentEvent);
                    events.emplace_back(currentEvent);
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
