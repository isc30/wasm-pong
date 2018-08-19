#pragma once

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/html5.h>
#endif

namespace emscripten
{
    using Callback = void(*)(void*);

    template<typename TCallable>
    Callback getCallback(const TCallable&)
    {
        static Callback executor = [](void* lambda)
        {
            (*static_cast<TCallable*>(lambda))();
        };

        return executor;
    }
}
