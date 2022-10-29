#pragma once

#include <CDX12/FrameResource.h>
#include <IPass.h>

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
            pack = std::move(p);
        }

    public:
        TransUpdatePass(std::string name = std::string("TransUpdatePass"));
        ~TransUpdatePass();

        void Init(ID3D12Device*) override;
        void Tick() override;

    private:
        PassPack pack;
    };
}
