#include <Pass/logical/UpdatePass.h>
#include <CDX12/Metalib.h>
#include <Utility/Macro.h>

using namespace Chen::RToy;
using namespace DirectX;

UpdatePass::UpdatePass(std::string name) : IPass(name) 
{
    AddObject(GetObjectMngr().GetObj("box1"));
    AddObject(GetObjectMngr().GetObj("sphere1"));
	AddObject(GetObjectMngr().GetObj("ground"));
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
	// PassConstants
	XMMATRIX view = pack.p2camera->GetView();
	XMMATRIX proj = pack.p2camera->GetProj();

	XMMATRIX viewProj = DirectX::XMMatrixMultiply(view, proj);
	XMMATRIX invView = DirectX::XMMatrixInverse(get_rvalue_ptr(XMMatrixDeterminant(view)), view);
	XMMATRIX invProj = DirectX::XMMatrixInverse(get_rvalue_ptr(XMMatrixDeterminant(proj)), proj);
	XMMATRIX invViewProj = DirectX::XMMatrixInverse(get_rvalue_ptr(XMMatrixDeterminant(viewProj)), viewProj);

	static PassConstants passCB;

	XMStoreFloat4x4(&passCB.View, DirectX::XMMatrixTranspose(view));
	XMStoreFloat4x4(&passCB.InvView, DirectX::XMMatrixTranspose(invView));
	XMStoreFloat4x4(&passCB.Proj, DirectX::XMMatrixTranspose(proj));
	XMStoreFloat4x4(&passCB.InvProj, DirectX::XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&passCB.ViewProj, DirectX::XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&passCB.InvViewProj, DirectX::XMMatrixTranspose(invViewProj));
	passCB.EyePosW = pack.p2camera->GetPosition3f();
	passCB.RenderTargetSize = XMFLOAT2(pack.width, pack.height);
	passCB.InvRenderTargetSize = XMFLOAT2(1.0f / pack.width, 1.0f / pack.height);
	passCB.NearZ = 1.0f;
	passCB.FarZ = 1000.0f;
	passCB.TotalTime = pack.p2timer->TotalTime();
	passCB.DeltaTime = pack.p2timer->DeltaTime();

    pack.currFrameResource->GetResource<std::shared_ptr<Chen::CDX12::UploadBuffer<PassConstants>>>(
        "PassCB")->CopyData(0, passCB);


	// Material Buffer
	for (auto &p2obj : mObjects)
	{
		// Update the transform property of all objects
		if (p2obj.second->GetProperty("Material")->IsDirty())
		{
			auto objImpl = p2obj.second->GetPropertyImpl<Material>("Material");
			Material::Impl new_impl(objImpl);
			auto mat = new_impl.material;

			BasicMaterialData matData;
			XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);
			matData.DiffuseAlbedo = mat->DiffuseAlbedo;
			matData.FresnelR0 = mat->FresnelR0;
			matData.Roughness = mat->Roughness;
			XMStoreFloat4x4(&matData.MatTransform, DirectX::XMMatrixTranspose(matTransform));
			matData.DiffuseMapIndex = mat->DiffuseSrvHeapIndex;
			matData.NormalMapIndex = mat->NormalSrvHeapIndex;

			pack.currFrameResource->GetResource<std::shared_ptr<Chen::CDX12::UploadBuffer<BasicMaterialData>>>(
				"MaterialData")->CopyData(mat->MatIndex, matData);
			p2obj.second->GetProperty("Material")->ClearOne();
		}
	}
}
