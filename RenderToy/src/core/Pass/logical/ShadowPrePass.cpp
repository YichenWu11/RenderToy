#include <Pass/logical/ShadowPrePass.h>
#include <Pass/logical/UpdatePass.h>
#include <PropertyMngr/Transform.h>
#include <PropertyMngr/Material.h>
#include <PropertyMngr/Mesh.h>
#include <Utility/Macro.h>
#include <Utility/GlobalParam.h>
#include <memory>

using namespace Chen::RToy;
using namespace DirectX;

ShadowPrePass::ShadowPrePass(std::string name) : IPass(name) 
{
	mSceneBounds.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mSceneBounds.Radius = sqrtf(70.0f * 70.0f + 70.0f * 70.0f);

    AddObject(GetObjectMngr().GetObj("box1"));
    AddObject(GetObjectMngr().GetObj("box2"));
    AddObject(GetObjectMngr().GetObj("box3"));
    AddObject(GetObjectMngr().GetObj("sphere1"));
    AddObject(GetObjectMngr().GetObj("ground"));
}

ShadowPrePass::~ShadowPrePass()
{

}

void ShadowPrePass::Init(ID3D12Device* _device)
{
    device = _device;
    mShadowMap = std::make_unique<ShadowMap>(device, 4096 * 4, 4096 * 4);
}

void ShadowPrePass::Tick()
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
    is_sm_init = true;

    TickShadowTransform();
    // update the shadow passCB
    TickShadowPrePassCB();

    // Draw Shadow Map Part
    pack.mCmdList->SetGraphicsRootSignature(
        GetRenderRsrcMngr().GetShaderMngr()->GetShader("ShadowShader")->RootSig());
    auto matBuffer =
        pack.currFrameResource->GetResource<std::shared_ptr<UploadBuffer<BasicMaterialData>>>("MaterialData")->GetResource();
    GetRenderRsrcMngr().GetShaderMngr()->GetShader("ShadowShader")->SetResource(
        "Materials", pack.mCmdList.Get(), matBuffer->GetGPUVirtualAddress());
    GetRenderRsrcMngr().GetShaderMngr()->GetShader("ShadowShader")->SetResource(
        "Textures", pack.mCmdList.Get(), GetRenderRsrcMngr().GetTexMngr()->GetTexAllocation().GetGpuHandle(0));

    DrawToShadowMap();
}

void ShadowPrePass::TickShadowTransform()
{
    	//// Only the first "main" light casts a shadow.
	DirectX::XMVECTOR lightDir = DirectX::XMLoadFloat3(&mBaseLightDirections[0]);
	DirectX::XMVECTOR lightPos = -2.0f * mSceneBounds.Radius * lightDir;
	DirectX::XMVECTOR targetPos = DirectX::XMLoadFloat3(&mSceneBounds.Center);
	DirectX::XMVECTOR lightUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMMATRIX lightView = DirectX::XMMatrixLookAtLH(lightPos, targetPos, lightUp);

	DirectX::XMStoreFloat3(&mLightPosW, lightPos);

	// Transform bounding sphere to light space.
	DirectX::XMFLOAT3 sphereCenterLS;
	DirectX::XMStoreFloat3(&sphereCenterLS, DirectX::XMVector3TransformCoord(targetPos, lightView));

	// Ortho frustum in light space encloses scene.
	float l = sphereCenterLS.x - mSceneBounds.Radius;
	float b = sphereCenterLS.y - mSceneBounds.Radius;
	float n = sphereCenterLS.z - mSceneBounds.Radius;
	float r = sphereCenterLS.x + mSceneBounds.Radius;
	float t = sphereCenterLS.y + mSceneBounds.Radius;
	float f = sphereCenterLS.z + mSceneBounds.Radius;

	mLightNearZ = n;
	mLightFarZ = f;
	DirectX::XMMATRIX lightProj = DirectX::XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	DirectX::XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	DirectX::XMMATRIX S = lightView * lightProj * T;
	DirectX::XMStoreFloat4x4(&mLightView, lightView);
	DirectX::XMStoreFloat4x4(&mLightProj, lightProj);
	DirectX::XMStoreFloat4x4(&mShadowTransform, S);

    GlobalParam::GetInstance().SetShadowTransform(mShadowTransform);
}

void ShadowPrePass::TickShadowPrePassCB()
{
    UpdatePass::PassConstants mShadowPrePassCB;

    DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&mLightView);
	DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&mLightProj);

	DirectX::XMMATRIX viewProj = DirectX::XMMatrixMultiply(view, proj);
	DirectX::XMMATRIX invView = DirectX::XMMatrixInverse(get_rvalue_ptr(DirectX::XMMatrixDeterminant(view)), view);
	DirectX::XMMATRIX invProj = DirectX::XMMatrixInverse(get_rvalue_ptr(DirectX::XMMatrixDeterminant(proj)), proj);
	DirectX::XMMATRIX invViewProj = DirectX::XMMatrixInverse(get_rvalue_ptr(DirectX::XMMatrixDeterminant(viewProj)), viewProj);

	UINT w = mShadowMap->Width();
	UINT h = mShadowMap->Height();

	DirectX::XMStoreFloat4x4(&mShadowPrePassCB.View, DirectX::XMMatrixTranspose(view));
	DirectX::XMStoreFloat4x4(&mShadowPrePassCB.InvView, DirectX::XMMatrixTranspose(invView));
	DirectX::XMStoreFloat4x4(&mShadowPrePassCB.Proj, DirectX::XMMatrixTranspose(proj));
	DirectX::XMStoreFloat4x4(&mShadowPrePassCB.InvProj, DirectX::XMMatrixTranspose(invProj));
	DirectX::XMStoreFloat4x4(&mShadowPrePassCB.ViewProj, DirectX::XMMatrixTranspose(viewProj));
	DirectX::XMStoreFloat4x4(&mShadowPrePassCB.InvViewProj, DirectX::XMMatrixTranspose(invViewProj));
	mShadowPrePassCB.EyePosW = mLightPosW;
	mShadowPrePassCB.RenderTargetSize = DirectX::XMFLOAT2((float)w, (float)h);
	mShadowPrePassCB.InvRenderTargetSize = DirectX::XMFLOAT2(1.0f / w, 1.0f / h);
	mShadowPrePassCB.NearZ = mLightNearZ;
	mShadowPrePassCB.FarZ = mLightFarZ;

    pack.currFrameResource->GetResource<std::shared_ptr<Chen::CDX12::UploadBuffer<UpdatePass::PassConstants>>>(
        "PassCB")->CopyData(1, mShadowPrePassCB);
}

void ShadowPrePass::DrawToShadowMap()
{
    pack.mCmdList.RSSetViewport(mShadowMap->Viewport());
    pack.mCmdList.RSSetScissorRect(mShadowMap->ScissorRect());

    pack.mCmdList.ResourceBarrierTransition(
        mShadowMap->Resource(),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        D3D12_RESOURCE_STATE_DEPTH_WRITE);

    UINT passCBByteSize = DXUtil::CalcConstantBufferByteSize(sizeof(UpdatePass::PassConstants));

    pack.mCmdList.ClearDepthStencilView(mShadowMap->Dsv());

    pack.mCmdList->OMSetRenderTargets(0, nullptr, false, get_rvalue_ptr(mShadowMap->Dsv()));

    auto passCB = 
        pack.currFrameResource->GetResource<std::shared_ptr<UploadBuffer<UpdatePass::PassConstants>>>("PassCB")->GetResource();
    D3D12_GPU_VIRTUAL_ADDRESS passCBAddress = passCB->GetGPUVirtualAddress() + 1 * passCBByteSize;
    GetRenderRsrcMngr().GetShaderMngr()->GetShader("ShadowShader")->SetResource(
        "PassCB", pack.mCmdList.Get(), passCBAddress);

    pack.mCmdList->SetPipelineState(GetRenderRsrcMngr().GetPSOMngr()->GetPipelineState("ShadowMap"));

    DrawObjects();

    pack.mCmdList.ResourceBarrierTransition(
        mShadowMap->Resource(),
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        D3D12_RESOURCE_STATE_GENERIC_READ);
}

void ShadowPrePass::DrawObjects()
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
