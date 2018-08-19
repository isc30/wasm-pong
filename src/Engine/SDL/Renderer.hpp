#pragma once

#include <Engine/SDL/Object.hpp>
#include <Engine/SDL/Surface.hpp>
#include <SDL.h>

namespace isc
{
    namespace sdl
    {
        class Renderer
        {
        public:

            Renderer();

        private:

            sdl::Object<SDL_Renderer> _renderer;
            Surface _surface;
        };
    }
}
