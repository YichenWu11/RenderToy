#pragma once

#include <IPass.h>

namespace Chen::RToy {
    class MatUpdatePass : public IPass
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
        MatUpdatePass(std::string name = std::string("MatUpdatePass"));
        ~MatUpdatePass();

        void Init(ID3D12Device*) override;
        void Tick() override;


    private:
        PassPack pack;
    };
}
