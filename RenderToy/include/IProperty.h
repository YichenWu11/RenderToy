#pragma once

#include <CDX12/Math/MathHelper.h>
#include <string>
#include <any>

namespace Chen::RToy {
    class IProperty
    {
    public:
        IProperty(std::string n) : name(n) {}
        virtual ~IProperty() {};
        IProperty(const IProperty&) = default;
        IProperty& operator=(const IProperty&) = default;
        IProperty(IProperty&&) = default;
        IProperty& operator=(IProperty&&) = default;

        std::string GetName() { return name; }
        virtual std::any GetImpl() = 0;

        void SetDirty() { NumFramesDirty = 3; }
        void ClearOne() { --NumFramesDirty; }
        bool IsDirty()  { return NumFramesDirty > 0; }

    protected:
        std::string name;
        int NumFramesDirty = 3;
    };
}
