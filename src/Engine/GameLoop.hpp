#pragma once

#include <memory>
#include <chrono>

#include <Engine/Common.hpp>
#include <Engine/Integrations/Emscripten.hpp>

template<typename TGameContext, typename... TArgs>
constexpr int initGameLoop(TArgs&&... args)
{
    auto context = std::make_unique<TGameContext>(std::forward<TArgs>(args)...);
    context->init();

    auto previousTime = std::chrono::steady_clock::now();
    DeltaTime deltaTime = std::chrono::nanoseconds(0);

#ifdef __EMSCRIPTEN__
    auto loopCallback = [&]() -> void
    {
        auto newTime = std::chrono::steady_clock::now();
        deltaTime = newTime - previousTime;
        previousTime = newTime;

        if (!context->loop(deltaTime))
        {
            context.reset();
            emscripten_cancel_main_loop();
        }
    };

    emscripten_set_main_loop_arg(
        emscripten::getCallback(loopCallback),
        &loopCallback,
        0, true);
#else
    do
    {
        auto newTime = std::chrono::steady_clock::now();
        deltaTime = newTime - previousTime;
        previousTime = newTime;
    }
    while (context->loop(deltaTime));

    context.reset();
#endif

    return 0;
}
