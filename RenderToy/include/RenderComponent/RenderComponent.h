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

        void FillPack(std::any _pack) override 
        {
            auto p = std::any_cast<ComPack>(_pack);
            pack = std::move(p);
        }

        void FillPassPack() override;

        void Init(ID3D12Device*, ID3D12GraphicsCommandList*) override;
        void Tick() override;

        struct ComPack
        {
            
        };

    private:
        ComPack pack;
    };
}
