#pragma once

namespace Chen::RToy {
    class IComponent 
    {
    public:
        IComponent() = default;
        IComponent(const IComponent&) = delete;
        IComponent& operator=(const IComponent&) = delete;
        virtual ~IComponent() {}

        virtual void Init() = 0;
        virtual void Tick() = 0;
    };
}
