#include "Window.hpp"

#include <SDL.h>

#include <Engine/SDL/Object.hpp>
#include <Engine/SDL/EventQueue.hpp>
#include <Engine/Graphics/OpenGL/OpenGL.hpp>

namespace isc
{
    Window::Window()
        : _window(sdl::makeNullObject<SDL_Window>())
        , _glContext(sdl::makeNullObject<void>())
    {
    }

    void Window::create(const char* title,
        const vec2<uint32_t>& size,
        const uint32_t flags)
    {
        _state.size = size + vec2<uint32_t>{1, 0};
        _window = sdl::makeObject<SDL_Window>(SDL_CreateWindow, SDL_DestroyWindow,
            title,
            static_cast<int32_t>(SDL_WINDOWPOS_CENTERED), static_cast<int32_t>(SDL_WINDOWPOS_CENTERED),
            static_cast<int32_t>(_state.size.x), static_cast<int32_t>(_state.size.y),
            SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | flags);

        setInitialWindowSize(size);

        initOpenGL();
        configure();
    }

    void Window::setInitialWindowSize(const vec2<uint32_t>& size)
    {
        bool isResizedExternally = sdl::EventQueue::any([](const SDL_Event& event)
        {
            return event.type == SDL_WINDOWEVENT
                && event.window.event == SDL_WINDOWEVENT_RESIZED;
        });

        // don't resize if the window was already resized externally
        if (!isResizedExternally)
        {
            SDL_SetWindowSize(_window.get(), static_cast<int32_t>(size.x), static_cast<int32_t>(size.y));
            SDL_SetWindowPosition(_window.get(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
        }
    }

    void Window::initOpenGL()
    {
#ifndef __EMSCRIPTEN__
        // WebGL 2.0 == OpenGL ES 3.0
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);

        _glContext = sdl::makeObject<void>(SDL_GL_CreateContext, SDL_GL_DeleteContext, _window.get());

        opengl::link();
    }

    void Window::configure()
    {
        GL(glViewport(0, 0, _state.size.x, _state.size.y));

        GL(glEnable(GL_BLEND));
        GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        GL(glEnable(GL_DEPTH_TEST));
        GL(glDepthFunc(GL_LEQUAL));

        GL(glEnable(GL_CULL_FACE));
        GL(glCullFace(GL_BACK));
        GL(glFrontFace(GL_CCW));

        SDL_GL_SetSwapInterval(0);
    }

    bool Window::handleEvent(const SDL_Event& event)
    {
        if (event.type != SDL_WINDOWEVENT)
        {
            return false;
        }

        const auto& windowEvent = event.window;

        switch (windowEvent.event)
        {
            case SDL_WINDOWEVENT_RESIZED:
            case SDL_WINDOWEVENT_SIZE_CHANGED:
            {
                _state.size = { windowEvent.data1, windowEvent.data2 };
                GL(glViewport(0, 0, _state.size.x, _state.size.y));

                break;
            }

            case SDL_WINDOWEVENT_FOCUS_GAINED:
            {
                _state.isFocused = true;
                break;
            }

            case SDL_WINDOWEVENT_FOCUS_LOST:
            {
                _state.isFocused = false;
                break;
            }

            case SDL_WINDOWEVENT_SHOWN:
            {
                _state.isVisible = true;
                break;
            }

            case SDL_WINDOWEVENT_HIDDEN:
            {
                _state.isVisible = false;
                break;
            }

            case SDL_WINDOWEVENT_CLOSE:
            {
                close();
                break;
            }

            default: break;
        }

        return true;
    }

    void Window::swap()
    {
        SDL_GL_SwapWindow(_window.get());
    }

    const vec2<uint32_t>& Window::getSize() const noexcept
    {
        return _state.size;
    }

    bool Window::isOpen() const noexcept
    {
        return _state.isOpen;
    }

    void Window::close() noexcept
    {
        _state.isOpen = false;

#ifndef __EMSCRIPTEN__
        hide();
#endif
    }

    bool Window::isVisible() const noexcept
    {
        return _state.isVisible;
    }

    void Window::show() noexcept
    {
        SDL_ShowWindow(_window.get());
        _state.isVisible = true;
    }

    void Window::hide() noexcept
    {
        SDL_HideWindow(_window.get());
        _state.isVisible = false;
    }

    bool Window::isFocused() const noexcept
    {
        return _state.isFocused;
    }
}
