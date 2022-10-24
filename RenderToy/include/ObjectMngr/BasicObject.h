#pragma once

/*
    BasicObject:
        Properties:
            - Transform  [ Default ]
            - Material   [ SetMaterial(BasicMaterial mat) ]
            - Mesh       [ SetMeshGeo(std::string) ]
*/

#include "../IObject.h"
#include "../PropertyMngr/PropertyMngr.h"

namespace Chen::RToy {
    enum class ObjectLayer : uint8_t
    {
        Opaque = 0,
        Transparent,
        Sky,
        Count
    };

    class BasicObject : public IObject
    {
    public:
        BasicObject(std::string name) : IObject(name) 
        {
            AddProperty(PropertyMngr::GetInstance().CreateAndGetProperty(PropertyMngr::Option::Mesh));
            AddProperty(PropertyMngr::GetInstance().CreateAndGetProperty(PropertyMngr::Option::Transform));
            AddProperty(PropertyMngr::GetInstance().CreateAndGetProperty(PropertyMngr::Option::Material));
        }
        BasicObject(const BasicObject&) = delete;
        BasicObject& operator=(const BasicObject&) = delete;
        BasicObject(BasicObject&&) = default;
        BasicObject& operator=(BasicObject&&) = default;

        bool IsVisible() { return visible; }
        void SetVisible() { visible = true; }
        void SetInvisible() { visible = false; }

    private:
        bool visible {true};
        DirectX::BoundingBox Bounds;
    };
}