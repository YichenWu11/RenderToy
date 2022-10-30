#pragma once

#include <IPass.h>
#include <Pass/logical/shadow/ShadowMap.h>
#include <Pass/logical/UpdatePass.h>
#include <CDX12/GCmdList.h>
#include <CDX12/FrameResource.h>
#include <ObjectMngr/BasicObject.h>

namespace Chen::RToy {
    class ShadowPass : public IPass
    {
    public:
        struct PassPack
        {
            Chen::CDX12::GCmdList mCmdList;
            Chen::CDX12::FrameResource* currFrameResource;
            ID3D12Resource* currBackBuffer;
            D3D12_CPU_DESCRIPTOR_HANDLE currBackBufferView;
            D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView;  
            D3D12_CPU_DESCRIPTOR_HANDLE shadowDsv;  
            D3D12_VIEWPORT mScreenViewport;
            D3D12_RECT mScissorRect; 
        };

        void FillPack(std::any _pack) override 
        { 
            auto p = std::any_cast<PassPack>(_pack);
            pack = std::move(p);
        }

        ShadowPass(std::string name = std::string("ShadowPass"));
        ~ShadowPass();

        void Init(ID3D12Device*) override;
        void Tick() override;

        void DrawObjects(ObjectLayer layer = ObjectLayer::Opaque);
        void DrawToShadowMap();
    private:
        PassPack pack;

        std::unique_ptr<ShadowMap> mShadowMap;
        DirectX::BoundingSphere mSceneBounds;
    };
}
