#include <Pass/logical/ShadowPrePass.h>
#include <Pass/logical/UpdatePass.h>
#include <PropertyMngr/Transform.h>
#include <PropertyMngr/Material.h>
#include <PropertyMngr/Mesh.h>
#include <Utility/Macro.h>
#include <memory>

using namespace Chen::RToy;
using namespace DirectX;

ShadowPass::ShadowPass(std::string name) : IPass(name) 
{
    mShadowMap = std::make_unique<ShadowMap>(device, 4096*4, 4096*4);
	mSceneBounds.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mSceneBounds.Radius = sqrtf(37.0f * 37.0f + 37.0f * 37.0f);

    AddObject(GetObjectMngr().GetObj("box1"));
    AddObject(GetObjectMngr().GetObj("box2"));
    AddObject(GetObjectMngr().GetObj("box3"));
    AddObject(GetObjectMngr().GetObj("sphere1"));
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
    static bool is_sm_init = false;
    if (!is_sm_init)
    {
        mShadowMap->BuildDescriptors(
            CD3DX12_CPU_DESCRIPTOR_HANDLE(GetRenderRsrcMngr().GetTexMngr()->GetTexAllocation().GetCpuHandle(
                GetRenderRsrcMngr().GetTexMngr()->GetSMIndex()
            )),
            CD3DX12_GPU_DESCRIPTOR_HANDLE(GetRenderRsrcMngr().GetTexMngr()->GetTexAllocation().GetGpuHandle(
                GetRenderRsrcMngr().GetTexMngr()->GetSMIndex()
            )),
            CD3DX12_CPU_DESCRIPTOR_HANDLE(pack.shadowDsv));
    }
    is_sm_init = false;

    // Draw Shadow Map Part

    
    
}

void ShadowPass::DrawToShadowMap()
{

}

void ShadowPass::DrawObjects(ObjectLayer layer)
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

