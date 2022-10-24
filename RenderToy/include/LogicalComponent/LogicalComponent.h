#pragma once

#include "../IComponent.h"
#include "../IPass.h"

namespace Chen::RToy {
    class LogicalComponent final : public IComponent
    {
    public:
        LogicalComponent() = default;
        LogicalComponent(const LogicalComponent&) = delete;
        LogicalComponent& operator=(const LogicalComponent&) = delete;
        ~LogicalComponent();

        void Init(ID3D12Device*, ID3D12GraphicsCommandList*) override;
        void Tick() override;

    private:
    };
}
