#pragma once

#include <IPass.h>
#include <Pass/logical/UpdatePass.h>
#include <ObjectMngr/BasicObject.h>

namespace Chen::RToy {
    class ShadowPrePass : public IPass
    {
    public:
        struct PassPack
        {
            Chen::CDX12::FrameResource* currFrameResource;
            D3D12_CPU_DESCRIPTOR_HANDLE shadowDsv;  
        };

        void FillPack(std::any _pack) override 
        { 
            auto p = std::any_cast<PassPack>(_pack);
            pack = std::move(p);
        }

        ShadowPrePass(std::string name = std::string("ShadowPrePass"));
        ~ShadowPrePass();

        void Init(ID3D12Device*) override;
        void Tick() override;

        void TickShadowTransform();
        void TickShadowPrePassCB();
    private:
        PassPack pack;

        DirectX::BoundingSphere mSceneBounds;
        float mLightNearZ = 0.0f;
        float mLightFarZ = 0.0f;
        DirectX::XMFLOAT3 mLightPosW;
        DirectX::XMFLOAT4X4 mLightView = Math::MathHelper::Identity4x4();
        DirectX::XMFLOAT4X4 mLightProj = Math::MathHelper::Identity4x4();
        DirectX::XMFLOAT4X4 mShadowTransform = Math::MathHelper::Identity4x4();
    };
}
