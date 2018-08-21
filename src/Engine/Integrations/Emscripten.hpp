#pragma once

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/html5.h>
#endif

#include <Engine/IO/FileSystem.hpp>
#include <Engine/Extensions/Callable.hpp>

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

    using AsyncGetCallback = void(const char*);

    template<typename TOnSuccess, typename TOnError>
    constexpr void prepareFile(
        const char* path,
        const Callable<TOnSuccess, AsyncGetCallback>& onSuccess,
        const Callable<TOnError, AsyncGetCallback>& onError)
    {
#ifdef __EMSCRIPTEN__
        emscripten_async_wget(path, path, callableToPointer(onSuccess), callableToPointer(onError));
#else
        isc::FileSystem::fileExists(path)
            ? onSuccess(path)
            : onError(path);
#endif
    }
}
