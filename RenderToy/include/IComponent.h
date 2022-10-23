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
        virtual void Update() = 0;
        virtual void Populate() = 0; // Populate might be Draw or other interface for various component
    };
}
