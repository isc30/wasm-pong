#pragma once

#include <memory>

#include <Engine/SDL/Object.hpp>
#include <Engine/Math/Vector.hpp>
#include <SDL.h>

namespace isc
{
    namespace sdl
    {
        class Surface
        {
        public:

            Surface();
            void create(const vec2<uint32_t>& size);

        private:

            sdl::Object<SDL_Surface> _surface;

            Surface(SDL_Surface* rawSurface); // gets ownership
        };
    }
}

