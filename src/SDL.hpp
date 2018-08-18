#pragma once

#include <memory>

#include "RuntimeException.hpp"

namespace isc
{
    template<typename TObject, typename... TArgs>
    using SDLObjectCreate = TObject*(*)(std::decay_t<TArgs>...);

    template<typename TObject>
    using SDLObjectDestroy = void(*)(TObject*);

    template<typename TObject>
    using SDLObject = std::unique_ptr<TObject, SDLObjectDestroy<TObject>>;

    template<typename TObject>
    SDLObject<TObject> make_object(
        TObject* instance,
        const SDLObjectDestroy<TObject> destroy)
    {
        if (instance == nullptr)
        {
            throw RuntimeException("Error creating SDLObject", SDL_GetError());
        }

        return SDLObject<TObject>(instance, destroy);
    }

    template<typename TObject, typename... TArgs>
    SDLObject<TObject> make_object(
        const SDLObjectCreate<TObject, TArgs...> create,
        const SDLObjectDestroy<TObject> destroy,
        TArgs&&... args)
    {
        return make_object(
            create(std::forward<TArgs>(args)...),
            destroy);
    }

    template<typename TObject>
    SDLObject<TObject> null_object()
    {
        return SDLObject<TObject>(nullptr, [](TObject*) {});
    }
}
