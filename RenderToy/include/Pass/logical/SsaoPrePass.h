#pragma once

#include <IPass.h>
#include <Pass/logical/UpdatePass.h>
#include <ObjectMngr/BasicObject.h>

namespace Chen::RToy {
    class SsaoPrePass : public IPass
    {
    public:
        struct SsaoConstants
        {
            DirectX::XMFLOAT4X4 Proj;
            DirectX::XMFLOAT4X4 InvProj;
            DirectX::XMFLOAT4X4 ProjTex;
            DirectX::XMFLOAT4   OffsetVectors[14];

            // For SsaoBlur.hlsl
            DirectX::XMFLOAT4 BlurWeights[3];

            DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };

            // Coordinates given in view space.
            float OcclusionRadius = 0.5f;
            float OcclusionFadeStart = 0.2f;
            float OcclusionFadeEnd = 2.0f;
            float SurfaceEpsilon = 0.05f;
        };
        
    public:
        struct PassPack
        {
            Chen::CDX12::FrameResource* currFrameResource;
            Chen::CDX12::Camera* p2camera;
            ID3D12Resource* mDepthStencilBuffer;
            D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
            UINT rtvDescriptorSize;
        };

        void FillPack(std::any _pack) override 
        { 
            auto p = std::any_cast<PassPack>(_pack);
            pack = std::move(p);
        }

        SsaoPrePass(std::string name = std::string("SsaoPrePass"));
        ~SsaoPrePass();

        void Init(ID3D12Device*) override;
        void Tick() override;

    private:
        PassPack pack;
    }; 
}
