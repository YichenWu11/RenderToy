#pragma once

#include "../../IPass.h"
#include <CDX12/FrameResource.h>
#include <PropertyMngr/Transform.h>

namespace Chen::RToy {
    class TransUpdatePass : public IPass
    {
    public:
        struct PassPack
        {
            Chen::CDX12::FrameResource* currFrameResource; 
        };

        void FillPack(std::any _pack) override 
        { 
            auto p = std::any_cast<PassPack>(_pack);
            pack = p;
        }

        TransUpdatePass(std::string name = std::string("TransUpdatePass"));
        ~TransUpdatePass();

        void Init(ID3D12Device*, ID3D12GraphicsCommandList*) override;
        void Tick() override;

    private:
        PassPack pack;
    };
}
