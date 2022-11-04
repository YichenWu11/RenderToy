#include <Pass/render/SsaoPass.h>
#include <Pass/logical/UpdatePass.h>
#include <CDX12/Metalib.h>
#include <Utility/Macro.h>
#include <memory>

using namespace Chen::RToy;

SsaoPass::SsaoPass(std::string name) : IPass(name) 
{
    AddObject(GetObjectMngr().GetObj("box1"));
    AddObject(GetObjectMngr().GetObj("box2"));
    AddObject(GetObjectMngr().GetObj("box3"));
    AddObject(GetObjectMngr().GetObj("sphere1"));
    AddObject(GetObjectMngr().GetObj("ground"));
}

SsaoPass::~SsaoPass()
{

}

void SsaoPass::Init(ID3D12Device* _device)
{
    device = _device;
}

void SsaoPass::Tick()
{
    DrawNormalsAndDepth();
	pack.mCmdList->SetGraphicsRootSignature(GetRenderRsrcMngr().GetShaderMngr()->GetShader("SsaoShader")->RootSig());
	GetGlobalParam().GetSsao()->ComputeSsao(pack.mCmdList.Get(), pack.currFrameResource, 3);
}

void SsaoPass::DrawNormalsAndDepth()
{
	pack.mCmdList.RSSetViewport(pack.mScreenViewport);
	pack.mCmdList.RSSetScissorRect(pack.mScissorRect);

	auto normalMap = GetGlobalParam().GetSsao()->NormalMap();
	auto normalMapRtv = GetGlobalParam().GetSsao()->NormalMapRtv();

	// Change to RENDER_TARGET.
    pack.mCmdList.ResourceBarrierTransition(
        normalMap,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        D3D12_RESOURCE_STATE_RENDER_TARGET);

	// Clear the screen normal map and depth buffer.
	float clearValue[] = { 0.0f, 0.0f, 1.0f, 0.0f };
	pack.mCmdList->ClearRenderTargetView(normalMapRtv, clearValue, 0, nullptr);
	pack.mCmdList->ClearDepthStencilView(pack.depthStencilView, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	pack.mCmdList->OMSetRenderTargets(1, &normalMapRtv, true, &(pack.depthStencilView));

	pack.mCmdList.SetDescriptorHeaps(GetRenderRsrcMngr().GetTexMngr()->GetTexAllocation().GetDescriptorHeap());

	// Bind the constant buffer for this pass.
    auto passCB = 
        pack.currFrameResource->GetResource<std::shared_ptr<UploadBuffer<UpdatePass::PassConstants>>>("PassCB")->GetResource();
	GetRenderRsrcMngr().GetShaderMngr()->GetShader("SsaoShader")->SetResource(
        "SsaoPassCB", pack.mCmdList.Get(), passCB->GetGPUVirtualAddress());

	pack.mCmdList->SetPipelineState(GetRenderRsrcMngr().GetPSOMngr()->GetPipelineState("DrawNormals"));

	// Draw Objects

	// Change back to GENERIC_READ so we can read the texture in a shader.
    pack.mCmdList.ResourceBarrierTransition(
        normalMap,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_GENERIC_READ);
}

void SsaoPass::DrawObjects()
{

}
