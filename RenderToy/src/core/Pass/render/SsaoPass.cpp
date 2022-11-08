#include <Pass/render/SsaoPass.h>
#include <Pass/logical/UpdatePass.h>
#include <PropertyMngr/Mesh.h>
#include <PropertyMngr/Material.h>
#include <CDX12/Metalib.h>
#include <Utility/Macro.h>
#include <memory>

using namespace Chen::RToy;

SsaoPass::SsaoPass(std::string name) : IPass(name) 
{

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
    // FIXME: Blur results in the instability of fps
	GetGlobalParam().GetSsao()->ComputeSsao(pack.mCmdList.Get(), pack.currFrameResource, 0);
}

void SsaoPass::DrawNormalsAndDepth()
{
	pack.mCmdList.RSSetViewport(pack.mScreenViewport);
	pack.mCmdList.RSSetScissorRect(pack.mScissorRect);
    pack.mCmdList.SetDescriptorHeaps(GetRenderRsrcMngr().GetTexMngr()->GetTexAllocation().GetDescriptorHeap());

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

	// Bind the constant buffer for this pass.
    auto passCB = 
        pack.currFrameResource->GetResource<std::shared_ptr<UploadBuffer<UpdatePass::PassConstants>>>("PassCB")->GetResource();
	GetRenderRsrcMngr().GetShaderMngr()->GetShader("DrawNormalsShader")->SetResource(
        "PassCB", pack.mCmdList.Get(), passCB->GetGPUVirtualAddress());
    auto matBuffer =
        pack.currFrameResource->GetResource<std::shared_ptr<UploadBuffer<BasicMaterialData>>>("MaterialData")->GetResource();
    GetRenderRsrcMngr().GetShaderMngr()->GetShader("DrawNormalsShader")->SetResource(
        "Materials", pack.mCmdList.Get(), matBuffer->GetGPUVirtualAddress());

    GetRenderRsrcMngr().GetShaderMngr()->GetShader("DrawNormalsShader")->SetResource(
        "Textures", pack.mCmdList.Get(), GetRenderRsrcMngr().GetTexMngr()->GetTexAllocation().GetGpuHandle(0));

	pack.mCmdList->SetPipelineState(GetRenderRsrcMngr().GetPSOMngr()->GetPipelineState("DrawNormals"));

	// Draw Objects
    DrawObjects();

	// Change back to GENERIC_READ so we can read the texture in a shader.
    pack.mCmdList.ResourceBarrierTransition(
        normalMap,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_GENERIC_READ);
}

void SsaoPass::DrawObjects()
{
    UINT objCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(Transform::Impl));
    UINT matCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(Material::ID));

    auto& objectCB =
        pack.currFrameResource->GetResource<std::shared_ptr<UploadBuffer<Transform::Impl>>>("ObjTransformCB");

    auto& matIdxCB =
        pack.currFrameResource->GetResource<std::shared_ptr<UploadBuffer<Material::ID>>>("MatIndexCB");

    for (auto& obj : mObjects)
    {
        if (static_cast<BasicObject*>(obj.second)->GetLayer() == ObjectLayer::Opaque)
        {
            auto mesh = obj.second->GetPropertyImpl<Mesh>("Mesh");
            pack.mCmdList->IASetVertexBuffers(0, 1, get_rvalue_ptr(mesh.pMesh->VertexBufferView()));
            pack.mCmdList->IASetIndexBuffer(get_rvalue_ptr(mesh.pMesh->IndexBufferView()));
            pack.mCmdList->IASetPrimitiveTopology(mesh.PrimitiveType);

            D3D12_GPU_VIRTUAL_ADDRESS transCBAddress = objectCB->GetResource()->GetGPUVirtualAddress() + (obj.second->GetID() - 1) * objCBByteSize;
            D3D12_GPU_VIRTUAL_ADDRESS matIdxCBAddress = matIdxCB->GetResource()->GetGPUVirtualAddress() + (obj.second->GetID() - 1) * matCBByteSize;
            GetRenderRsrcMngr().GetShaderMngr()->GetShader("DrawNormalsShader")->SetResource("ObjTransformCB", pack.mCmdList.Get(), transCBAddress);
            GetRenderRsrcMngr().GetShaderMngr()->GetShader("DrawNormalsShader")->SetResource("MatIndexCB", pack.mCmdList.Get(), matIdxCBAddress);

            pack.mCmdList->DrawIndexedInstanced(mesh.IndexCount, 1, mesh.StartIndexLocation, mesh.BaseVertexLocation, 0);
        }
    }
}
