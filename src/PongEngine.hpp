#pragma once

#include <SDL.h>

#include <Engine/GameLoop.hpp>
#include <Engine/IO/Window.hpp>
#include <Engine/Graphics/OpenGL/OpenGL.hpp>
#include <Engine/SDL/EventQueue.hpp>

struct PongEngine
{
    isc::Window window;
    SDL_Renderer* renderer;
    SDL_Surface* surface;

    PongEngine()
    {
        window.create("Pong", { 640, 480 });
    }

    void init()
    {
        opengl::printContext();
    }

    ~PongEngine()
    {
    }

    bool loop(DeltaTime deltaTime)
    {
        SDL_Event event;

        while (isc::sdl::EventQueue::poll(event))
        {
            window.handleEvent(event);
        }

        if (!window.isOpen())
        {
            return false;
        }

        render(deltaTime);

        return true;
    }

    void render(DeltaTime deltaTime)
    {
        GL(glClearColor(0.f, 0x33 / 255.f, 0x66 / 255.f, 1.f));
        GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        window.swap();
    }
};
