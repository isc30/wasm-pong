#pragma once

#include <type_traits>
#include <functional>

template<typename TCallable, typename TSignature>
constexpr bool is_callable_as_v = std::is_constructible<std::function<TSignature>, TCallable>::value;

template<
    typename TCallable,
    typename TSignature,
    typename = std::enable_if_t<is_callable_as_v<TCallable, TSignature>>>
using Callable = TCallable;

template<typename TCallable>
struct CallableMetadata
    : CallableMetadata<decltype(&TCallable::operator())>
{
};

template<class TClass, typename TReturn, typename... TArgs>
struct CallableMetadata<TReturn(TClass::*)(TArgs...) const>
{
    using class_t = TClass;
    using return_t = TReturn;
    using args_tuple_t = std::tuple<TArgs...>;
    using ptr_t = TReturn(*)(TArgs...);

    static ptr_t generatePointer(const TClass& instance)
    {
        static TClass staticInstanceCopy = instance;

        return [](TArgs... args)
        {
            return staticInstanceCopy(args...);
        };
    }
};

template<typename TCallable>
auto* callableToPointer(const TCallable& callable)
{
    return CallableMetadata<TCallable>::generatePointer(callable);
}
