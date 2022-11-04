#pragma once

#include <IPass.h>
#include <Pass/logical/ssao/Ssao.h>
#include <Pass/logical/UpdatePass.h>
#include <ObjectMngr/BasicObject.h>

namespace Chen::RToy {
    class SsaoPass : public IPass
    {
    public:
        struct PassPack
        {
            Chen::CDX12::GCmdList mCmdList;
            Chen::CDX12::FrameResource* currFrameResource; 
            D3D12_VIEWPORT mScreenViewport;
            D3D12_RECT mScissorRect; 
            D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView;
        };

        void FillPack(std::any _pack) override 
        { 
            auto p = std::any_cast<PassPack>(_pack);
            pack = std::move(p);
        }

        SsaoPass(std::string name = std::string("SsaoPass"));
        ~SsaoPass();

        void Init(ID3D12Device*) override;
        void Tick() override;

        void DrawNormalsAndDepth();
        void DrawObjects();
    private:
        PassPack pack;
    };
}
