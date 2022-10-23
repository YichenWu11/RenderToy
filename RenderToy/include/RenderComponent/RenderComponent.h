#pragma once

#include "../IComponent.h"
#include "../IObject.h"
#include "../IPass.h"
#include <CDX12/DXUtil.h>

namespace Chen::RToy {
    class RenderComponent final : public IComponent
    {
    public:
        RenderComponent() = default;
        RenderComponent(const RenderComponent&) = delete;
        RenderComponent& operator=(const RenderComponent&) = delete;
        ~RenderComponent();

        void Init() override;
        void Update() override;
        void Populate() override;

    private:
        /*
            The initial of All objects.
            GUID ---> Object
        */
        std::map<std::uint32_t, std::shared_ptr<IObject>> mObjects; 
        
        std::map<std::string, std::unique_ptr<IPass>> mPasses;
    };
}
