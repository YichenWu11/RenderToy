#include <Pass/render/ShadowPass.h>
#include <Pass/logical/UpdatePass.h>
#include <PropertyMngr/Transform.h>
#include <PropertyMngr/Material.h>
#include <PropertyMngr/Mesh.h>
#include <CDX12/Metalib.h>
#include <Utility/Macro.h>
#include <Utility/GlobalParam.h>
#include <memory>

using namespace Chen::RToy;

ShadowPass::ShadowPass(std::string name) : IPass(name) 
{
    AddObject(GetObjectMngr().GetObj("box1"));
    AddObject(GetObjectMngr().GetObj("box2"));
    AddObject(GetObjectMngr().GetObj("box3"));
    AddObject(GetObjectMngr().GetObj("sphere1"));
    AddObject(GetObjectMngr().GetObj("ground"));
}

ShadowPass::~ShadowPass()
{

}

void ShadowPass::Init(ID3D12Device* _device)
{
    device = _device;
}

void ShadowPass::Tick()
{
    // Draw Shadow Map Part
    pack.mCmdList->SetGraphicsRootSignature(
        GetRenderRsrcMngr().GetShaderMngr()->GetShader("ShadowShader")->RootSig());
    auto matBuffer =
        pack.currFrameResource->GetResource<std::shared_ptr<UploadBuffer<BasicMaterialData>>>("MaterialData")->GetResource();
    GetRenderRsrcMngr().GetShaderMngr()->GetShader("ShadowShader")->SetResource(
        "Materials", pack.mCmdList.Get(), matBuffer->GetGPUVirtualAddress());

    DrawToShadowMap();
}

void ShadowPass::DrawToShadowMap()
{
    pack.mCmdList.RSSetViewport(GlobalParam::GetInstance().GetShadowMap()->Viewport());
    pack.mCmdList.RSSetScissorRect(GlobalParam::GetInstance().GetShadowMap()->ScissorRect());

    pack.mCmdList.ResourceBarrierTransition(
        GlobalParam::GetInstance().GetShadowMap()->Resource(),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        D3D12_RESOURCE_STATE_DEPTH_WRITE);

    UINT passCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(UpdatePass::PassConstants));

    pack.mCmdList.ClearDepthStencilView(GlobalParam::GetInstance().GetShadowMap()->Dsv());

    pack.mCmdList->OMSetRenderTargets(0, nullptr, false, get_rvalue_ptr(GlobalParam::GetInstance().GetShadowMap()->Dsv()));

    auto passCB = 
        pack.currFrameResource->GetResource<std::shared_ptr<UploadBuffer<UpdatePass::PassConstants>>>("PassCB")->GetResource();
    D3D12_GPU_VIRTUAL_ADDRESS passCBAddress = passCB->GetGPUVirtualAddress() + 1 * passCBByteSize;
    GetRenderRsrcMngr().GetShaderMngr()->GetShader("ShadowShader")->SetResource(
        "PassCB", pack.mCmdList.Get(), passCBAddress);

    pack.mCmdList->SetPipelineState(GetRenderRsrcMngr().GetPSOMngr()->GetPipelineState("ShadowMap"));

    DrawObjects();

    pack.mCmdList.ResourceBarrierTransition(
        GlobalParam::GetInstance().GetShadowMap()->Resource(),
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        D3D12_RESOURCE_STATE_GENERIC_READ);
}

void ShadowPass::DrawObjects()
{
    UINT objCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(Transform::Impl));
    UINT matCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(Material::ID));

    auto& objectCB = 
        pack.currFrameResource->GetResource<std::shared_ptr<UploadBuffer<Transform::Impl>>>("ObjTransformCB");

    auto& matIdxCB = 
        pack.currFrameResource->GetResource<std::shared_ptr<UploadBuffer<Material::ID>>>("MatIndexCB");

    for (auto& obj : mObjects)
    {
        auto mesh = obj.second->GetPropertyImpl<Mesh>("Mesh");
        pack.mCmdList->IASetVertexBuffers(0, 1, get_rvalue_ptr(mesh.pMesh->VertexBufferView()));
        pack.mCmdList->IASetIndexBuffer(get_rvalue_ptr(mesh.pMesh->IndexBufferView()));
        pack.mCmdList->IASetPrimitiveTopology(mesh.PrimitiveType);

        D3D12_GPU_VIRTUAL_ADDRESS transCBAddress = objectCB->GetResource()->GetGPUVirtualAddress() + (obj.second->GetID()-1) * objCBByteSize;
        D3D12_GPU_VIRTUAL_ADDRESS matIdxCBAddress = matIdxCB->GetResource()->GetGPUVirtualAddress() + (obj.second->GetID()-1) * matCBByteSize;
        GetRenderRsrcMngr().GetShaderMngr()->GetShader("ShadowShader")->SetResource("ObjTransformCB", pack.mCmdList.Get(), transCBAddress);
        GetRenderRsrcMngr().GetShaderMngr()->GetShader("ShadowShader")->SetResource("MatIndexCB", pack.mCmdList.Get(), matIdxCBAddress);

        pack.mCmdList->DrawIndexedInstanced(mesh.IndexCount, 1, mesh.StartIndexLocation, mesh.BaseVertexLocation, 0);    
    }
}
