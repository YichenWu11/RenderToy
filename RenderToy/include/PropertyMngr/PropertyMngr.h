#pragma once

#include "./Material.h"
#include "./Transform.h"
#include "./Mesh.h"
#include <vector>
#include <memory>

namespace Chen::RToy {
    class PropertyMngr
    {
    public:
        enum class Option : uint8_t
        {
            Transform = 0,
            Material,
            Mesh,
            Count
        };

        static PropertyMngr& GetInstance()
        {
            static PropertyMngr instance;
            return instance;
        }

        IProperty* CreateAndGetProperty(Option option)
        {
            IProperty* p2property = nullptr;

            switch(option)
            {
                case Option::Transform:
                    p2property = new Transform();
                    break;
                case Option::Material:
                    p2property = new Material();
                    break;
                case Option::Mesh:
                    p2property = new Mesh();
                    break;
                default:
                    break;
            }

            AllProperties.push_back(p2property);

            return p2property;
        }

    private:
        PropertyMngr() = default;
        ~PropertyMngr()
        {
            for (auto &p : AllProperties)
            {
                if (p != nullptr) delete p;
            }
        }   
        std::vector<IProperty*> AllProperties;             
    };
}
