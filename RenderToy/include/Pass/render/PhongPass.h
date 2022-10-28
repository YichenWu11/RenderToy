#pragma once

#include <IPass.h>
#include <CDX12/GCmdList.h>
#include <CDX12/FrameResource.h>

using namespace Chen::CDX12;

namespace Chen::RToy {
    class PhongPass : public IPass
    {
    public:
        struct PassPack
        {
            Chen::CDX12::GCmdList mCmdList;
            Chen::CDX12::FrameResource* currFrameResource;
            ID3D12Resource* currBackBuffer;
            D3D12_CPU_DESCRIPTOR_HANDLE currBackBufferView;
            D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView;  
            D3D12_VIEWPORT mScreenViewport;
            D3D12_RECT mScissorRect; 
        };

        void FillPack(std::any _pack) override 
        { 
            auto p = std::any_cast<PassPack>(_pack);
            pack = std::move(p);
        }

        PhongPass(std::string name = std::string("PhongPass"));
        ~PhongPass();

        void Init(ID3D12Device*, ID3D12GraphicsCommandList*) override;
        void Tick() override;

        void DrawObjects();

    private:
        PassPack pack;
    };
}
