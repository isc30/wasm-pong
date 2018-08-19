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
