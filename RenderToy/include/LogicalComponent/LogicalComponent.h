#pragma once

#include "../IComponent.h"
#include "../IPass.h"
#include <CDX12/DXUtil.h>

namespace Chen::RToy {
    class LogicalComponent final : public IComponent
    {
    public:
        LogicalComponent() = default;
        LogicalComponent(const LogicalComponent&) = delete;
        LogicalComponent& operator=(const LogicalComponent&) = delete;
        ~LogicalComponent();

        void Init() override;
        void Update() override;
        void Populate() override;

    private:
        std::map<std::string, std::unique_ptr<IPass>> mPasses;
    };
}
