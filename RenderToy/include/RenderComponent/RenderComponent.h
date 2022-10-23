#pragma once

#include "../IComponent.h"
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
        std::map<std::string, std::unique_ptr<IPass>> mPasses;
    };
}
