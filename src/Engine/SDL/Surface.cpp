#include "Surface.hpp"

namespace isc
{
    namespace sdl
    {
        Surface::Surface()
            : _surface(sdl::makeNullObject<SDL_Surface>())
        {
        }

        void Surface::create(const vec2<uint32_t>& size)
        {
            uint32_t rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            rmask = 0xff000000;
            gmask = 0x00ff0000;
            bmask = 0x0000ff00;
            amask = 0x000000ff;
#else
            rmask = 0x000000ff;
            gmask = 0x0000ff00;
            bmask = 0x00ff0000;
            amask = 0xff000000;
#endif

            SDL_Surface* surface = SDL_CreateRGBSurface(0, size.x, size.y, 32, rmask, gmask, bmask, amask);
            SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_BLEND);

            _surface = sdl::makeObject<SDL_Surface>(surface, SDL_FreeSurface);
        }

        Surface::Surface(SDL_Surface* rawSurface)
            : _surface(sdl::makeObject<SDL_Surface>(rawSurface, SDL_FreeSurface))
        {
        }
    }
}
