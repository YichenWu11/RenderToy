#include <Pass/logical/UpdatePass.h>
#include <CDX12/Metalib.h>

using namespace Chen::RToy;
using namespace DirectX;

UpdatePass::UpdatePass(std::string name) : IPass(name) 
{

}

UpdatePass::~UpdatePass()
{

}

void UpdatePass::Init(ID3D12Device* _device)
{
	device = _device;
}

void UpdatePass::Tick()
{
	XMMATRIX view = pack.p2camera->GetView();
	XMMATRIX proj = pack.p2camera->GetProj();

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(get_rvalue_ptr(XMMatrixDeterminant(view)), view);
	XMMATRIX invProj = XMMatrixInverse(get_rvalue_ptr(XMMatrixDeterminant(proj)), proj);
	XMMATRIX invViewProj = XMMatrixInverse(get_rvalue_ptr(XMMatrixDeterminant(viewProj)), viewProj);

	PassConstants passCB;

	XMStoreFloat4x4(&passCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&passCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&passCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&passCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&passCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&passCB.InvViewProj, XMMatrixTranspose(invViewProj));
	passCB.EyePosW = pack.p2camera->GetPosition3f();
	passCB.RenderTargetSize = XMFLOAT2(pack.width, pack.height);
	passCB.InvRenderTargetSize = XMFLOAT2(1.0f / pack.width, 1.0f / pack.height);
	passCB.NearZ = 1.0f;
	passCB.FarZ = 1000.0f;
	passCB.TotalTime = pack.p2timer->TotalTime();
	passCB.DeltaTime = pack.p2timer->DeltaTime();

    pack.currFrameResource->GetResource<std::shared_ptr<Chen::CDX12::UploadBuffer<PassConstants>>>(
        "PassCB")->CopyData(0, passCB);
}
