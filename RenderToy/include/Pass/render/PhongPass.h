#pragma once

#include "../../IPass.h"

using namespace Chen::CDX12;

namespace Chen::RToy {
    class PhongPass : public IPass
    {
    public:
        PhongPass(std::string name = std::string("PhongPass"));
        ~PhongPass();

        void Init(ID3D12Device*, ID3D12GraphicsCommandList*) override;
        void Tick() override;
    };
}
