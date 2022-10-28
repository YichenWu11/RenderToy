#pragma once

#include <IPass.h>

namespace Chen::RToy {
    class MeshUpdatePass : public IPass
    {
    public:
        struct PassPack
        {

        };

        void FillPack(std::any _pack) override 
        { 
            auto p = std::any_cast<PassPack>(_pack);
            pack = std::move(p);
        }

    public:
        MeshUpdatePass(std::string name = std::string("MeshUpdatePass"));
        ~MeshUpdatePass();

        void Init(ID3D12Device*, ID3D12GraphicsCommandList*) override;
        void Tick() override; 

    private:
        PassPack pack;
    };
}
