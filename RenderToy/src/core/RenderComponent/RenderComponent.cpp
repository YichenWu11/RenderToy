#include <RenderComponent/RenderComponent.h>
#include <Pass/render/PhongPass.h>

using namespace Chen::RToy;

RenderComponent::RenderComponent()
{
    AddPass(std::move(std::make_unique<PhongPass>()));
}

RenderComponent::~RenderComponent()
{

}

void RenderComponent::Init(ID3D12Device* _device, ID3D12GraphicsCommandList* _cmdList)
{
    mPasses["PhongPass"]->Init(_device, _cmdList);
}

void RenderComponent::Tick()
{
    mPasses["PhongPass"]->Tick();
}

void RenderComponent::FillPassPack()
{
    PhongPass::PassPack pack1;
    pack1.mCmdList = pack.mCmdList;
    pack1.currFrameResource = pack.currFrameResource;
    pack1.currBackBuffer = pack.currBackBuffer;
    pack1.currBackBufferView = pack.currBackBufferView;
    pack1.depthStencilView = pack.depthStencilView;
    pack1.mScissorRect = pack.mScissorRect;
	pack1.mScreenViewport = pack.mScreenViewport;

    mPasses["PhongPass"]->FillPack(pack1);
}
