#pragma once

#include "../IProperty.h"
#include <CDX12/Math/MathHelper.h>

using namespace Chen::CDX12::Math;

namespace Chen::RToy {
    class Transform : public IProperty
    {
    public:
        Transform(std::string n = std::string("Transform")) : IProperty(n) {}
        ~Transform() = default;
        
        void SetRotation(DirectX::XMFLOAT4X4 r)  { impl.Rotation = r; }
        void SetScale(DirectX::XMFLOAT4X4 s)     { impl.Scale = s; }
        void SetTranslate(DirectX::XMFLOAT4X4 t) { impl.Translate = t; }

        DirectX::XMFLOAT4X4 GetRotation() { return impl.Rotation; }
        DirectX::XMFLOAT4X4 GetScale() { return impl.Scale; }
        DirectX::XMFLOAT4X4 GetTranslate() { return impl.Translate; }

        std::any GetImpl() override { return impl; }

        void Tick(std::any params) override 
        {
            Impl new_impl = std::any_cast<Impl>(params);
            SetRotation(new_impl.Rotation);
            SetScale(new_impl.Scale);
            SetTranslate(new_impl.Translate);
        }

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
