#pragma once

#include "IObject.h"
#include <vector>
#include <memory>

namespace Chen::RToy {
    class IPass
    {
    public:
        IPass() = default;
        IPass(const IPass&) = delete;
        IPass& operator=(const IPass&) = delete;
        virtual ~IPass() {}

        virtual void Init() = 0;
        virtual void Update() = 0;
        virtual void Populate() = 0; // Populate might be Draw or other interface for various component

    private:
        std::vector<std::weak_ptr<IObject>> mObjects;
    };
}