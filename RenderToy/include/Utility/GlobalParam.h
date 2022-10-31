#pragma once

#include <DirectXMath.h>
#include <CDX12/Math/MathHelper.h>

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

    private:
        GlobalParam() = default;
        ~GlobalParam() = default;

        DirectX::XMFLOAT4X4 mShadowTransform = Chen::CDX12::Math::MathHelper::Identity4x4();
    };
}
