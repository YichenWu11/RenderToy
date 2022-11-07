#include <LogicalComponent/LogicalComponent.h>
#include <Pass/logical/UpdatePass.h>
#include <Pass/logical/TransUpdatePass.h>
#include <Pass/logical/MeshUpdatePass.h>
#include <Pass/logical/MatUpdatePass.h>
#include <Pass/logical/ShadowPrePass.h>
#include <Pass/logical/SsaoPrePass.h>
#include <Utility/Macro.h>

using namespace Chen::RToy;

LogicalComponent::LogicalComponent()
{  
    AddPass(std::move(std::make_unique<ShadowPrePass>()));
    AddPass(std::move(std::make_unique<UpdatePass>()));  
    AddPass(std::move(std::make_unique<SsaoPrePass>()));
    AddPass(std::move(std::make_unique<TransUpdatePass>()));
    AddPass(std::move(std::make_unique<MeshUpdatePass>()));
    AddPass(std::move(std::make_unique<MatUpdatePass>()));
}

LogicalComponent::~LogicalComponent()
{

}

void LogicalComponent::Init(ID3D12Device* _device)
{
    for (auto& pass : mPasses)
    {
        pass.second->Init(_device);
    }

    FillObjectsForPass();
}

void LogicalComponent::Tick()
{
    for (auto& pass : mPasses)
    {
        pass.second->Tick();
    }
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

    // ShadowPrePass
    static ShadowPrePass::PassPack pack5;
    pack5.currFrameResource = pack.currFrameResource;
    pack5.shadowDsv = pack.shadowDsv;
    mPasses[std::string("ShadowPrePass")]->FillPack(pack5);

    // SsaoPrePass
    static SsaoPrePass::PassPack pack6;
    pack6.currFrameResource = pack.currFrameResource;
    pack6.p2camera = pack.p2camera;
    pack6.mDepthStencilBuffer = pack.mDepthStencilBuffer;
	pack6.rtvHandle = pack.rtvHandle;
    pack6.rtvDescriptorSize = pack.rtvDescriptorSize;
    mPasses[std::string("SsaoPrePass")]->FillPack(pack6);
}

void LogicalComponent::FillObjectsForPass()
{
    for (auto& pass : mPasses)
    {
        for (int idx = 1; idx <= GetObjectMngr().GetBiggestID(); ++idx)
        {
            pass.second->AddObject(GetObjectMngr().GetObj(idx));
        }   
    }
}
