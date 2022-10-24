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
        virtual void Tick() = 0;

    private:
        std::vector<std::weak_ptr<IObject>> mObjects;
    };
}