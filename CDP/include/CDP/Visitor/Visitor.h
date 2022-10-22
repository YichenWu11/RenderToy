#pragma once

#include <map>
#include <type_traits>
#include <typeindex>
#include <functional>

namespace Chen 
{
    template<typename Impl, typename Base>
    class Visitor
    {
    public:
        template<typename Base_>
        void Visit(Base_* ptrBase)
        {
            callbacks[std::type_index(typeid(*ptrBase))](ptrBase);
        }
    protected:
        template<typename Derived>
        void Register()
        {
            callbacks[std::type_index(typeid(Derived))] = 
                [impl = static_cast<Impl*>(this)](Base* ptrBase) {
                    constexpr void(Impl::*memberFunc)(Derived*) = &Impl::ImplVisit;
                    (impl->*memberFunc)(static_cast<Derived*>(ptrBase));
                };
        }

    private:
        std::map<std::type_index, std::function<void(Base*)>> callbacks;
    };
}

#include "details/Visitor.inl"