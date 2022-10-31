#include <Pass/logical/UpdatePass.h>
#include <CDX12/Metalib.h>
#include <Utility/Macro.h>
#include <Utility/GlobalParam.h>

using namespace Chen::RToy;
using namespace DirectX;

UpdatePass::UpdatePass(std::string name) : IPass(name) 
{
    AddObject(GetObjectMngr().GetObj("box1"));
	AddObject(GetObjectMngr().GetObj("box2"));
	AddObject(GetObjectMngr().GetObj("box3"));
    AddObject(GetObjectMngr().GetObj("sphere1"));
	AddObject(GetObjectMngr().GetObj("ground"));
	AddObject(GetObjectMngr().GetObj("skyBox"));
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

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX viewProjTex = XMMatrixMultiply(viewProj, T);
	XMMATRIX shadowTransform = XMLoadFloat4x4(get_rvalue_ptr((GlobalParam::GetInstance().GetShadowTransform())));;

	static PassConstants passCB;

	DirectX::XMStoreFloat4x4(&passCB.View, DirectX::XMMatrixTranspose(view));
	DirectX::XMStoreFloat4x4(&passCB.InvView, DirectX::XMMatrixTranspose(invView));
	DirectX::XMStoreFloat4x4(&passCB.Proj, DirectX::XMMatrixTranspose(proj));
	DirectX::XMStoreFloat4x4(&passCB.InvProj, DirectX::XMMatrixTranspose(invProj));
	DirectX::XMStoreFloat4x4(&passCB.ViewProj, DirectX::XMMatrixTranspose(viewProj));
	DirectX::XMStoreFloat4x4(&passCB.InvViewProj, DirectX::XMMatrixTranspose(invViewProj));
	DirectX::XMStoreFloat4x4(&passCB.ViewProjTex, DirectX::XMMatrixTranspose(viewProjTex));
	DirectX::XMStoreFloat4x4(&passCB.ShadowTransform, DirectX::XMMatrixTranspose(shadowTransform));
	passCB.EyePosW = pack.p2camera->GetPosition3f();
	passCB.RenderTargetSize = DirectX::XMFLOAT2(pack.width, pack.height);
	passCB.InvRenderTargetSize = DirectX::XMFLOAT2(1.0f / pack.width, 1.0f / pack.height);
	passCB.NearZ = 1.0f;
	passCB.FarZ = 1000.0f;
	passCB.TotalTime = pack.p2timer->TotalTime();
	passCB.DeltaTime = pack.p2timer->DeltaTime();

	passCB.Lights[0].Direction = DirectX::XMFLOAT3(0.57735f, -0.87735f, 0.57735f);
	passCB.Lights[0].Strength = { 0.6f, 0.6f, 0.6f };
	passCB.Lights[1].Direction = DirectX::XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);
	passCB.Lights[1].Strength = { 0.1f, 0.1f, 0.1f };
	passCB.Lights[2].Direction = DirectX::XMFLOAT3(0.0f, -0.707f, -0.707f);
	passCB.Lights[2].Strength = { 0.1f, 0.1f, 0.1f };

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
			DirectX::XMStoreFloat4x4(&matData.MatTransform, DirectX::XMMatrixTranspose(matTransform));
			matData.DiffuseMapIndex = mat->DiffuseSrvHeapIndex;
			matData.NormalMapIndex = mat->NormalSrvHeapIndex;

			pack.currFrameResource->GetResource<std::shared_ptr<Chen::CDX12::UploadBuffer<BasicMaterialData>>>(
				"MaterialData")->CopyData(mat->MatIndex, matData);
			p2obj.second->GetProperty("Material")->ClearOne();
		}
	}
}
