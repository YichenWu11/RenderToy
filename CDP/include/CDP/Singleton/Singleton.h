#pragma once

#include <type_traits>

namespace Chen
{
    template<typename T>
    class Singleton
    {
    protected:
        Singleton() = default;

    public:
        static inline T& getInstance() noexcept(std::is_nothrow_constructible<T>::value);

        virtual ~Singleton() noexcept = default;

        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;
    };
}

#include "details/Singleton.inl"