#pragma once

#include "../IComponent.h"
#include "../IPass.h"
#include <CDX12/GCmdList.h>
#include <CDX12/FrameResource.h>

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
            FillPassPack();
        }

        void FillPassPack() override;

        void Init(ID3D12Device*, ID3D12GraphicsCommandList*) override;
        void Tick() override;

        struct ComPack
        {
            Chen::CDX12::GCmdList mCmdList;
            Chen::CDX12::FrameResource* currFrameResource;
            ID3D12Resource* currBackBuffer{nullptr};
            D3D12_CPU_DESCRIPTOR_HANDLE currBackBufferView{0};
            D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView{0};
            D3D12_VIEWPORT mScreenViewport;
            D3D12_RECT mScissorRect; 
        };

    private:
        ComPack pack;
    };
}
