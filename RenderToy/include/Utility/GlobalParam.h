#pragma once

#include <DirectXMath.h>
#include <CDX12/Math/MathHelper.h>
#include <Pass/logical/shadow/ShadowMap.h>

using namespace DirectX;

namespace Chen::RToy {
    class GlobalParam
    {
    public:
        static GlobalParam& GetInstance()
        {
            static GlobalParam instance;
            return instance;
        }

        void SetShadowTransform(DirectX::XMFLOAT4X4 mat) { mShadowTransform = mat; }
        DirectX::XMFLOAT4X4 GetShadowTransform() { return mShadowTransform; }

        void Init(ID3D12Device* _device)
        {
            mShadowMap = std::make_unique<ShadowMap>(_device, 4096 * 4, 4096 * 4);
        }

        ShadowMap* GetShadowMap() { return mShadowMap.get(); }

    private:
        GlobalParam() = default;
        ~GlobalParam() = default;

        DirectX::XMFLOAT4X4 mShadowTransform = Chen::CDX12::Math::MathHelper::Identity4x4();
        std::unique_ptr<ShadowMap> mShadowMap;
    };
}
