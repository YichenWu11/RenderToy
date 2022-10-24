#pragma once

/*
    Pass to Update All PassConstants
*/

#include "../../IPass.h"
#include <CDX12/FrameResource.h>
#include <CDX12/Common/Camera.h>
#include <CDX12/Common/GameTimer.h>

using namespace Chen::CDX12;

namespace Chen::RToy {
    class UpdatePass : public IPass
    {
    public:
        struct PassConstants
        {
            DirectX::XMFLOAT4X4 View = Math::MathHelper::Identity4x4();
            DirectX::XMFLOAT4X4 InvView = Math::MathHelper::Identity4x4();
            DirectX::XMFLOAT4X4 Proj = Math::MathHelper::Identity4x4();
            DirectX::XMFLOAT4X4 InvProj = Math::MathHelper::Identity4x4();
            DirectX::XMFLOAT4X4 ViewProj = Math::MathHelper::Identity4x4();
            DirectX::XMFLOAT4X4 InvViewProj = Math::MathHelper::Identity4x4();
            DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
            float cbPerObjectPad1 = 0.0f;  // For alignment
            DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
            DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
            float NearZ = 0.0f;
            float FarZ = 0.0f;
            float TotalTime = 0.0f;
            float DeltaTime = 0.0f;
        };

        struct PassPack
        {
            Chen::CDX12::FrameResource* currFrameResource;
            Chen::CDX12::Camera* p2camera;
            Chen::CDX12::GameTimer* p2timer;
            float width;
            float height;
        };  

        void FillPack(std::any _pack) override
        { 
            auto p = std::any_cast<PassPack>(_pack);
            pack = p;
        }

        UpdatePass(std::string name = std::string("UpdatePass"));
        ~UpdatePass();

        void Init(ID3D12Device*, ID3D12GraphicsCommandList*) override;
        void Tick() override;
    private:
        PassPack pack;
    };
}
