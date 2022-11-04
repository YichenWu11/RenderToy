#pragma once

#include <DirectXMath.h>
#include <CDX12/Math/MathHelper.h>
#include <Pass/logical/shadow/ShadowMap.h>
#include <Pass/logical/ssao/Ssao.h>
#include <Pass/logical/UpdatePass.h>

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

        void Init(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, UINT width, UINT height)
        {
            mShadowMap = std::make_unique<ShadowMap>(device, 4096 * 4, 4096 * 4);
            mSsao = std::make_unique<Ssao>(device, cmdList, width, height);
        }

        // ***************************************************************************************
        // Getters and Setters

        void SetShadowTransform(DirectX::XMFLOAT4X4 mat) { mShadowTransform = mat; }
        DirectX::XMFLOAT4X4 GetShadowTransform() { return mShadowTransform; }

        ShadowMap* GetShadowMap() { return mShadowMap.get(); }

        Ssao* GetSsao() { return mSsao.get(); }

        void SetMainLightDir(DirectX::XMFLOAT3 dir) { mBaseLightDirections[0] = dir; }
        DirectX::XMFLOAT3 GetMainLightDir() { return mBaseLightDirections[0]; }

        void SetMainPassData(UpdatePass::PassConstants m) { mMainPassCB = m; }
        UpdatePass::PassConstants GetMainPassData() { return mMainPassCB; }

        // ***************************************************************************************

    private:
        GlobalParam() = default;
        ~GlobalParam() = default;

        // ***************************************************************************************
        // For Shadow Map

        DirectX::XMFLOAT4X4 mShadowTransform = Chen::CDX12::Math::MathHelper::Identity4x4();

        std::unique_ptr<ShadowMap> mShadowMap;

        DirectX::XMFLOAT3 mBaseLightDirections[3] = {
            DirectX::XMFLOAT3(0.57735f, -0.87735f, 0.57735f),
            DirectX::XMFLOAT3(-0.57735f, -0.57735f, 0.57735f),
            DirectX::XMFLOAT3(0.0f, -0.707f, -0.707f)
        };

        // ***************************************************************************************

        // ***************************************************************************************
        // For SSAO

        std::unique_ptr<Ssao> mSsao;
        UpdatePass::PassConstants mMainPassCB;

        // ***************************************************************************************
    };
}
