#include <LogicalComponent/LogicalComponent.h>
#include <Pass/logical/UpdatePass.h>
#include <Pass/logical/TransUpdatePass.h>
#include <Pass/logical/MeshUpdatePass.h>
#include <Pass/logical/MatUpdatePass.h>

using namespace Chen::RToy;

LogicalComponent::LogicalComponent()
{
    AddPass(std::move(std::make_unique<UpdatePass>()));  
    AddPass(std::move(std::make_unique<TransUpdatePass>()));
    AddPass(std::move(std::make_unique<MeshUpdatePass>()));
    AddPass(std::move(std::make_unique<MatUpdatePass>()));
}

LogicalComponent::~LogicalComponent()
{

}

void LogicalComponent::Init(ID3D12Device* _device)
{
    mPasses[std::string("UpdatePass")]->Init(_device);
    mPasses[std::string("TransUpdatePass")]->Init(_device);
    mPasses[std::string("MeshUpdatePass")]->Init(_device);
    mPasses[std::string("MatUpdatePass")]->Init(_device);
}

void LogicalComponent::Tick()
{
    mPasses[std::string("UpdatePass")]->Tick();
    mPasses[std::string("TransUpdatePass")]->Tick();
    mPasses[std::string("MeshUpdatePass")]->Tick();
    mPasses[std::string("MatUpdatePass")]->Tick();
}

void LogicalComponent::FillPassPack()
{
    // UpdatePass
    static UpdatePass::PassPack pack1;
    pack1.currFrameResource = pack.currFrameResource;
    pack1.p2camera = pack.p2camera;
    pack1.p2timer = pack.p2timer;
    pack1.width = pack.width;
    pack1.height = pack.height;
    mPasses[std::string("UpdatePass")]->FillPack(pack1);

    // TransUpdatePass
    static TransUpdatePass::PassPack pack2;
    pack2.currFrameResource = pack.currFrameResource;
    mPasses[std::string("TransUpdatePass")]->FillPack(pack2);

    // MeshUpdatePass
    static MeshUpdatePass::PassPack pack3;
    mPasses[std::string("MeshUpdatePass")]->FillPack(pack3);

    // MatUpdatePass
    static MatUpdatePass::PassPack pack4;
    pack4.currFrameResource = pack.currFrameResource;
    mPasses[std::string("MatUpdatePass")]->FillPack(pack4);
}

void LogicalComponent::FillObjectsForPass()
{
    
}
