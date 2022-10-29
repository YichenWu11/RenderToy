#include <Pass/render/PhongPass.h>
#include <Pass/logical/UpdatePass.h>
#include <PropertyMngr/Transform.h>
#include <PropertyMngr/Material.h>
#include <PropertyMngr/Mesh.h>
#include <Utility/Macro.h>
#include <memory>

using namespace Chen::RToy;

PhongPass::PhongPass(std::string name) : IPass(name) 
{
    AddObject(GetObjectMngr().GetObj("box1"));
    AddObject(GetObjectMngr().GetObj("sphere1"));
}

PhongPass::~PhongPass()
{

}

void PhongPass::Init(ID3D12Device* _device)
{
    device = _device;
}

void PhongPass::Tick()
{
    pack.mCmdList.RSSetViewport(pack.mScreenViewport);
    pack.mCmdList.RSSetScissorRect(pack.mScissorRect);

    pack.mCmdList.ResourceBarrierTransition(
        pack.currBackBuffer,
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);
    
    pack.mCmdList.ClearRenderTargetView(pack.currBackBufferView, DirectX::Colors::DarkGray);
    pack.mCmdList.ClearDepthStencilView(pack.depthStencilView);

    pack.mCmdList.OMSetRenderTarget(pack.currBackBufferView, pack.depthStencilView);

    /*
        // Render
    */
    pack.mCmdList->SetPipelineState(GetRenderRsrcMngr().GetPSOMngr()->GetPipelineState("Base"));

    // mCmdList.SetDescriptorHeaps(csuGpuDH.GetDescriptorHeap());
    pack.mCmdList->SetGraphicsRootSignature(
        GetRenderRsrcMngr().GetShaderMngr()->GetShader("IShader")->RootSig());

    auto passCB = 
        pack.currFrameResource->GetResource<std::shared_ptr<UploadBuffer<UpdatePass::PassConstants>>>("PassCB")->GetResource();
    GetRenderRsrcMngr().GetShaderMngr()->GetShader("IShader")->SetResource(
        "PassCB", pack.mCmdList.Get(), passCB->GetGPUVirtualAddress());

    DrawObjects();

    // *********************************

    pack.mCmdList.ResourceBarrierTransition(
        pack.currBackBuffer,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);
}

void PhongPass::DrawObjects()
{
    UINT objCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(Transform::Impl));

    auto& objectCB = 
        pack.currFrameResource->GetResource<std::shared_ptr<UploadBuffer<Transform::Impl>>>("ObjTransformCB");

    for (auto& obj : mObjects)
    {
        auto mesh = obj.second->GetPropertyImpl<Mesh>("Mesh");
        pack.mCmdList->IASetVertexBuffers(0, 1, get_rvalue_ptr(mesh.pMesh->VertexBufferView()));
        pack.mCmdList->IASetIndexBuffer(get_rvalue_ptr(mesh.pMesh->IndexBufferView()));
        pack.mCmdList->IASetPrimitiveTopology(mesh.PrimitiveType);

        D3D12_GPU_VIRTUAL_ADDRESS transCBAddress = objectCB->GetResource()->GetGPUVirtualAddress() + (obj.second->GetID()-1) * objCBByteSize;

        GetRenderRsrcMngr().GetShaderMngr()->GetShader("IShader")->SetResource("ObjTransformCB", pack.mCmdList.Get(), transCBAddress);
        pack.mCmdList->DrawIndexedInstanced(mesh.IndexCount, 1, mesh.StartIndexLocation, mesh.BaseVertexLocation, 0);
    }
}
