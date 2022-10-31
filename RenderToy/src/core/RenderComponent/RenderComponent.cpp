#include <RenderComponent/RenderComponent.h>
#include <Pass/render/PhongPass.h>
#include <Pass/render/ShadowPass.h>

using namespace Chen::RToy;

RenderComponent::RenderComponent()
{
    AddPass(std::move(std::make_unique<ShadowPass>()));
    AddPass(std::move(std::make_unique<PhongPass>()));
}

RenderComponent::~RenderComponent()
{

}

void RenderComponent::Init(ID3D12Device* _device)
{
    mPasses["ShadowPass"]->Init(_device);
    mPasses["PhongPass"]->Init(_device);
}

void RenderComponent::Tick()
{
    mPasses["ShadowPass"]->Tick();
    mPasses["PhongPass"]->Tick();
}

void RenderComponent::FillPassPack()
{
    static PhongPass::PassPack pack1;
    pack1.mCmdList = pack.mCmdList;
    pack1.currFrameResource = pack.currFrameResource;
    pack1.currBackBuffer = pack.currBackBuffer;
    pack1.currBackBufferView = pack.currBackBufferView;
    pack1.depthStencilView = pack.depthStencilView;
    pack1.mScissorRect = pack.mScissorRect;
	pack1.mScreenViewport = pack.mScreenViewport;

    mPasses["PhongPass"]->FillPack(pack1);

    static ShadowPass::PassPack pack2;
    pack2.mCmdList = pack.mCmdList;
    pack2.currFrameResource = pack.currFrameResource;
    pack2.shadowDsv = pack.shadowDsv;

    mPasses["ShadowPass"]->FillPack(pack2);
}

void RenderComponent::FillObjectsForPass()
{
    
}
