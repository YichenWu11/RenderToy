#pragma once

#include <IProperty.h>
#include <CDX12/Material/MaterialMngr.h>

using namespace Chen::CDX12::Math;
using namespace Chen::CDX12;

namespace Chen::RToy {
    class Material : public IProperty
    {
    public:
        Material(std::string n = std::string("Material")) : IProperty(n) {}
        ~Material() = default;

        void SetMaterial(BasicMaterial* mat) 
        { 
            impl.material = mat; 
            SetDirty(); 
        }

        void SetMatTransform(DirectX::XMFLOAT4X4 mat)
        {
            impl.material->MatTransform = mat;
            SetDirty();
        }

        std::string GetMatName() { return impl.material->Name; }
        int GetMatIndex() { return impl.material->MatIndex; }
        int GetDiffuse() { return impl.material->DiffuseSrvHeapIndex; }
        int GetNormal()  { return impl.material->NormalSrvHeapIndex; }
        DirectX::XMFLOAT4 GetAlbedo() { return impl.material->DiffuseAlbedo; }
        DirectX::XMFLOAT3 GetFresnelR0() { return impl.material->FresnelR0; }
        float GetRoughness() { return impl.material->Roughness; }
        DirectX::XMFLOAT4X4 GetMatTransform() { return impl.material->MatTransform; }

        std::any GetImpl() override { return impl; }

        struct Impl
        {
            BasicMaterial* material;
        };

        // special
        struct ID
        {
            UINT matIndex;
            /* for alignment */
            UINT Pad0;
            UINT Pad1;
            UINT Pad2;
        };

    private:
        Impl impl;
    };
}
