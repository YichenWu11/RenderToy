#include <LogicalComponent/LogicalComponent.h>
#include <Pass/logical/UpdatePass.h>
#include <Pass/logical/TransUpdatePass.h>
#include <Pass/logical/MeshUpdatePass.h>

using namespace Chen::RToy;

LogicalComponent::LogicalComponent()
{
    AddPass(std::move(std::make_unique<UpdatePass>()));  
    AddPass(std::move(std::make_unique<TransUpdatePass>()));
    AddPass(std::move(std::make_unique<MeshUpdatePass>()));
}

LogicalComponent::~LogicalComponent()
{

}

void LogicalComponent::Init(ID3D12Device* _device, ID3D12GraphicsCommandList* _cmdList)
{
    mPasses[std::string("UpdatePass")]->Init(_device, _cmdList);
    mPasses[std::string("TransUpdatePass")]->Init(_device, _cmdList);
    mPasses[std::string("MeshUpdatePass")]->Init(_device, _cmdList);
}

void LogicalComponent::Tick()
{
    mPasses[std::string("UpdatePass")]->Tick();
    mPasses[std::string("TransUpdatePass")]->Tick();
    mPasses[std::string("MeshUpdatePass")]->Tick();
}

void LogicalComponent::FillPassPack()
{
    // UpdatePass
    UpdatePass::PassPack pack1;
    pack1.currFrameResource = pack.currFrameResource;
    pack1.p2camera = pack.p2camera;
    pack1.p2timer = pack.p2timer;
    pack1.width = pack.width;
    pack1.height = pack.height;
    mPasses[std::string("UpdatePass")]->FillPack(pack1);

    // TransUpdatePass
    TransUpdatePass::PassPack pack2;
    pack2.currFrameResource = pack.currFrameResource;
    mPasses[std::string("TransUpdatePass")]->FillPack(pack2);

    // MeshUpdatePass
    MeshUpdatePass::PassPack pack3;
    mPasses[std::string("MeshUpdatePass")]->FillPack(pack3);
}
