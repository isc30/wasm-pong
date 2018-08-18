#pragma once

#include <string>

#include <SDL_video.h>
#include <SDL_events.h>
#include "SDL.hpp"

#include <glm/glm.hpp>

namespace isc
{
    template<typename TPredicate>
    using vec2 = glm::vec<2, TPredicate>;

    class Window
    {
    public:

        Window();
        virtual ~Window() = default;

        virtual void create(const char* title,
            const vec2<uint32_t>& size,
            const uint32_t flags = 0);

        const vec2<uint32_t>& getSize() const noexcept;

        bool isOpen() const noexcept;
        void close() noexcept;

        bool isVisible() const noexcept;
        void show() noexcept;
        void hide() noexcept;

        bool isFocused() const noexcept;

        virtual bool handleEvent(const SDL_Event& event);

        void swap();

    protected:

        struct State
        {
            bool isOpen = true;
            bool isVisible = true;
            bool isFocused = true;
            vec2<uint32_t> size;
        };

        State _state;

        virtual void configure();

    private:

        SDLObject<SDL_Window> _window;
        SDLObject<void> _glContext;

        void initOpenGL();
        void setInitialWindowSize(const vec2<uint32_t>& size);
    };
}
