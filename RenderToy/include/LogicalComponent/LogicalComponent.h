#pragma once

#include "../IComponent.h"
#include "../IPass.h"
#include <CDX12/FrameResource.h>
#include <CDX12/Common/Camera.h>
#include <CDX12/Common/GameTimer.h>

namespace Chen::RToy {
    class LogicalComponent final : public IComponent
    {
    public:
        LogicalComponent();
        LogicalComponent(const LogicalComponent&) = delete;
        LogicalComponent& operator=(const LogicalComponent&) = delete;
        ~LogicalComponent();

        void FillPack(std::any _pack) override 
        {
            auto p = std::any_cast<ComPack>(_pack);
            pack = std::move(p);
            FillPassPack();
        }

        void FillPassPack() override;

        void FillObjectsForPass() override;

        void Init(ID3D12Device*) override;
        void Tick() override;

        struct ComPack
        {
            Chen::CDX12::FrameResource* currFrameResource;
            Chen::CDX12::Camera* p2camera;
            Chen::CDX12::GameTimer* p2timer;
            float width;
            float height;
        };
        
    private:
        ComPack pack;
    };
}
