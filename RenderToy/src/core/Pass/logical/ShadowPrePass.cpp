#include <Pass/logical/ShadowPrePass.h>
#include <Pass/logical/UpdatePass.h>
#include <Utility/Macro.h>
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
}

void ShadowPrePass::Tick()
{
    // Init Once
    static bool is_sm_init = false;
    if (!is_sm_init)
    {
        GetGlobalParam().GetShadowMap()->BuildDescriptors(
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
    TickShadowPrePassCB();
}

void ShadowPrePass::TickShadowTransform()
{
    	//// Only the first "main" light casts a shadow.
	DirectX::XMVECTOR lightDir = DirectX::XMLoadFloat3(get_rvalue_ptr(GetGlobalParam().GetMainLightDir()));
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

    GetGlobalParam().SetShadowTransform(mShadowTransform);
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

	UINT w = GetGlobalParam().GetShadowMap()->Width();
	UINT h = GetGlobalParam().GetShadowMap()->Height();

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
