#pragma once

#include "../IProperty.h"
#include <CDX12/Math/MathHelper.h>
#include <CDX12/Material/MaterialMngr.h>

using namespace Chen::CDX12::Math;
using namespace Chen::CDX12;

namespace Chen::RToy {
    class Material : public IProperty
    {
    public:
        Material(std::string n = std::string("Material")) : IProperty(n) {}
        ~Material() = default;

        void SetDiffuse(int index) { impl.material.DiffuseSrvHeapIndex = index; }
        void SetNormal(int index)  { impl.material.NormalSrvHeapIndex  = index; }
        void SetAlbedo(DirectX::XMFLOAT4 albedo) { impl.material.DiffuseAlbedo = albedo; }
        void SetFresnelR0(DirectX::XMFLOAT3 r0) { impl.material.FresnelR0 = r0; }
        void SetRoughness(float r) { impl.material.Roughness = r; }
        void SetMatTransform(DirectX::XMFLOAT4X4 mat) { impl.material.MatTransform = mat; }
        void SetMaterial(BasicMaterial mat) { impl.material = mat; }

        std::string GetMatName() { return impl.material.Name; }
        int GetMatIndex() { return impl.material.MatIndex; }
        int GetDiffuse() { return impl.material.DiffuseSrvHeapIndex; }
        int GetNormal()  { return impl.material.NormalSrvHeapIndex; }
        DirectX::XMFLOAT4 GetAlbedo() { return impl.material.DiffuseAlbedo; }
        DirectX::XMFLOAT3 GetFresnelR0() { return impl.material.FresnelR0; }
        float GetRoughness() { return impl.material.Roughness; }
        DirectX::XMFLOAT4X4 GetMatTransform() { return impl.material.MatTransform; }

        std::any GetImpl() override { return impl; }

        void Tick(std::any params) override 
        {
            Impl new_impl = std::any_cast<Impl>(params);
            SetDiffuse(new_impl.material.DiffuseSrvHeapIndex);
            SetNormal(new_impl.material.NormalSrvHeapIndex);
            SetAlbedo(new_impl.material.DiffuseAlbedo);
            SetFresnelR0(new_impl.material.FresnelR0);
            SetRoughness(new_impl.material.Roughness);
            SetMatTransform(new_impl.material.MatTransform);
        }

        struct Impl
        {
            BasicMaterial material;
        };

    private:
        Impl impl;
    };
}
