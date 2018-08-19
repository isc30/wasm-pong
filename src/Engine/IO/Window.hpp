#pragma once

#include <string>

#include <SDL.h>

#include <Engine/SDL/Object.hpp>
#include <Engine/Math/Vector.hpp>

namespace isc
{
    class Window
    {
    public:

        Window();
        virtual ~Window() = default;

        virtual void create(const char* title,
            const vec2<uint32_t>& size,
            const SDL_WindowFlags flags = static_cast<SDL_WindowFlags>(0));

        const vec2<uint32_t>& getSize() const noexcept;

        bool isOpen() const noexcept;
        void close() noexcept;

        bool isVisible() const noexcept;
        void show() noexcept;
        void hide() noexcept;

        bool isFocused() const noexcept;

        virtual bool handleEvent(const SDL_Event& event);

        void requestFullScreen(bool borderless) noexcept;
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

        sdl::Object<SDL_Window> _window;
        sdl::Object<void> _glContext;

        void initOpenGL();
        void setInitialWindowSize(const vec2<uint32_t>& size);
    };
}
