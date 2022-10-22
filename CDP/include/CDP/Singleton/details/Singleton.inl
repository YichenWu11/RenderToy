#pragma once

#include "CDP/Singleton/Singleton.h"
#include <type_traits>

namespace Chen 
{
    template<typename T>
    T& Singleton<T>::getInstance() noexcept(std::is_nothrow_constructible<T>::value)
    {
        static T instance;
        return instance;
    }
}
