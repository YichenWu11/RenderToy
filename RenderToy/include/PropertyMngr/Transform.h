#pragma once

#include <IProperty.h>

using namespace Chen::CDX12::Math;

namespace Chen::RToy {
    class Transform : public IProperty
    {
    public:
        Transform(std::string n = std::string("Transform")) : IProperty(n) {}
        ~Transform() = default;
        
        void SetRotation(DirectX::XMFLOAT4X4 r)  { impl.Rotation = r; SetDirty(); }
        void SetScale(DirectX::XMFLOAT4X4 s)     { impl.Scale = s; SetDirty(); }
        void SetTranslate(DirectX::XMFLOAT4X4 t) { impl.Translate = t; SetDirty(); }

        DirectX::XMFLOAT4X4 GetRotation() { return impl.Rotation; }
        DirectX::XMFLOAT4X4 GetScale() { return impl.Scale; }
        DirectX::XMFLOAT4X4 GetTranslate() { return impl.Translate; }

        std::any GetImpl() override { return impl; }

        struct Impl
        {
            // World = Rotation * Scale * Translate.
            DirectX::XMFLOAT4X4 Rotation  {MathHelper::Identity4x4()};
            DirectX::XMFLOAT4X4 Scale     {MathHelper::Identity4x4()};
            DirectX::XMFLOAT4X4 Translate {MathHelper::Identity4x4()};      
        };
    private:
        Impl impl;
    };
}
