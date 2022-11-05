#include <Pass/render/PhongPass.h>
#include <Pass/logical/UpdatePass.h>
#include <Utility/Macro.h>
#include <memory>
#include <imgui_impl_dx12.h>

using namespace Chen::RToy;

PhongPass::PhongPass(std::string name) : IPass(name) 
{
    
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

    pack.mCmdList.SetDescriptorHeaps(GetRenderRsrcMngr().GetTexMngr()->GetTexAllocation().GetDescriptorHeap());

    pack.mCmdList->SetGraphicsRootSignature(
        GetRenderRsrcMngr().GetShaderMngr()->GetShader("IShader")->RootSig());

    UINT passCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(UpdatePass::PassConstants));
    auto passCB = 
        pack.currFrameResource->GetResource<std::shared_ptr<UploadBuffer<UpdatePass::PassConstants>>>("PassCB")->GetResource();
    D3D12_GPU_VIRTUAL_ADDRESS passCBAddress = passCB->GetGPUVirtualAddress() + 0 * passCBByteSize;
    GetRenderRsrcMngr().GetShaderMngr()->GetShader("IShader")->SetResource(
        "PassCB", pack.mCmdList.Get(), passCBAddress);

    auto matBuffer =
        pack.currFrameResource->GetResource<std::shared_ptr<UploadBuffer<BasicMaterialData>>>("MaterialData")->GetResource();
    GetRenderRsrcMngr().GetShaderMngr()->GetShader("IShader")->SetResource(
        "Materials", pack.mCmdList.Get(), matBuffer->GetGPUVirtualAddress());

    GetRenderRsrcMngr().GetShaderMngr()->GetShader("IShader")->SetResource(
        "Textures", pack.mCmdList.Get(), GetRenderRsrcMngr().GetTexMngr()->GetTexAllocation().GetGpuHandle(0));

    auto cubeTexDescriptor = GetRenderRsrcMngr().GetTexMngr()->GetTexAllocation().GetGpuHandle(
        GetRenderRsrcMngr().GetTexMngr()->GetCubeIndex());
    size_t idx = GetRenderRsrcMngr().GetTexMngr()->GetCubeIndex();
    GetRenderRsrcMngr().GetShaderMngr()->GetShader("IShader")->SetResource(
        "CubeMap", pack.mCmdList.Get(), cubeTexDescriptor);

    DrawObjects();

    pack.mCmdList->SetPipelineState(GetRenderRsrcMngr().GetPSOMngr()->GetPipelineState("Sky"));
    DrawObjects(ObjectLayer::Sky);

    pack.mCmdList->SetPipelineState(GetRenderRsrcMngr().GetPSOMngr()->GetPipelineState("Transparent"));
    DrawObjects(ObjectLayer::Transparent);

    // FIXME: render imgui in PhongPass
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), pack.mCmdList.Get());

    // *********************************

    pack.mCmdList.ResourceBarrierTransition(
        pack.currBackBuffer,
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);
}

void PhongPass::DrawObjects(ObjectLayer layer)
{
    UINT objCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(Transform::Impl));
    UINT matCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(Material::ID));

    auto& objectCB = 
        pack.currFrameResource->GetResource<std::shared_ptr<UploadBuffer<Transform::Impl>>>("ObjTransformCB");

    auto& matIdxCB = 
        pack.currFrameResource->GetResource<std::shared_ptr<UploadBuffer<Material::ID>>>("MatIndexCB");

    for (auto& obj : mObjects)
    {
        if (dynamic_cast<BasicObject*>(obj.second)->GetLayer() == layer)
        {
            auto mesh = obj.second->GetPropertyImpl<Mesh>("Mesh");
            pack.mCmdList->IASetVertexBuffers(0, 1, get_rvalue_ptr(mesh.pMesh->VertexBufferView()));
            pack.mCmdList->IASetIndexBuffer(get_rvalue_ptr(mesh.pMesh->IndexBufferView()));
            pack.mCmdList->IASetPrimitiveTopology(mesh.PrimitiveType);

            D3D12_GPU_VIRTUAL_ADDRESS transCBAddress = objectCB->GetResource()->GetGPUVirtualAddress() + (obj.second->GetID()-1) * objCBByteSize;
            D3D12_GPU_VIRTUAL_ADDRESS matIdxCBAddress = matIdxCB->GetResource()->GetGPUVirtualAddress() + (obj.second->GetID()-1) * matCBByteSize;

            if (layer == ObjectLayer::Opaque || layer == ObjectLayer::Transparent)
            {
                GetRenderRsrcMngr().GetShaderMngr()->GetShader("IShader")->SetResource("ObjTransformCB", pack.mCmdList.Get(), transCBAddress);
                GetRenderRsrcMngr().GetShaderMngr()->GetShader("IShader")->SetResource("MatIndexCB", pack.mCmdList.Get(), matIdxCBAddress);
            }
            else if (layer == ObjectLayer::Sky)
            {
                GetRenderRsrcMngr().GetShaderMngr()->GetShader("SkyShader")->SetResource("ObjTransformCB", pack.mCmdList.Get(), transCBAddress);
                GetRenderRsrcMngr().GetShaderMngr()->GetShader("SkyShader")->SetResource("MatIndexCB", pack.mCmdList.Get(), matIdxCBAddress);
            }
            else
            {
                /*
                    Other Layer
                */
            }
            pack.mCmdList->DrawIndexedInstanced(mesh.IndexCount, 1, mesh.StartIndexLocation, mesh.BaseVertexLocation, 0);
        }
    }
}
