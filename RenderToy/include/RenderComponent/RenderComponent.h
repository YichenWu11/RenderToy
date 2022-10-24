#pragma once

#include "../IComponent.h"
#include "../IPass.h"

namespace Chen::RToy {
    class RenderComponent final : public IComponent
    {
    public:
        RenderComponent();
        RenderComponent(const RenderComponent&) = delete;
        RenderComponent& operator=(const RenderComponent&) = delete;
        ~RenderComponent();

        void Init(ID3D12Device*, ID3D12GraphicsCommandList*) override;
        void Tick() override;

    private:
    };
}
