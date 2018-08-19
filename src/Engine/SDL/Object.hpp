#pragma once

#include <memory>

#include <Engine/Exceptions/RuntimeException.hpp>
#include <SDL.h>

namespace isc
{
    namespace sdl
    {
        template<typename TObject, typename... TArgs>
        using ObjectCreate = TObject*(*)(std::decay_t<TArgs>...);

        template<typename TObject>
        using ObjectDestroy = void(*)(TObject*);

        template<typename TObject>
        using Object = std::unique_ptr<TObject, ObjectDestroy<TObject>>;

        template<typename TObject>
        Object<TObject> makeObject(
            TObject* instance,
            const ObjectDestroy<TObject> destroy)
        {
            if (instance == nullptr)
            {
                throw RuntimeException("Error creating Object", SDL_GetError());
            }

            return Object<TObject>(instance, destroy);
        }

        template<typename TObject, typename... TArgs>
        Object<TObject> makeObject(
            const ObjectCreate<TObject, TArgs...> create,
            const ObjectDestroy<TObject> destroy,
            TArgs&&... args)
        {
            return makeObject(
                create(std::forward<TArgs>(args)...),
                destroy);
        }

        template<typename TObject>
        Object<TObject> makeNullObject()
        {
            static const auto destructor = [](TObject*) {};
            return Object<TObject>(nullptr, destructor);
        }
    }
}
