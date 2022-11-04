#include <Pass/logical/SsaoPrePass.h>
#include <Utility/Macro.h>

using namespace Chen::RToy;
using namespace DirectX;

SsaoPrePass::SsaoPrePass(std::string name) : IPass(name)
{
    AddObject(GetObjectMngr().GetObj("box1"));
    AddObject(GetObjectMngr().GetObj("box2"));
    AddObject(GetObjectMngr().GetObj("box3"));
    AddObject(GetObjectMngr().GetObj("sphere1"));
    AddObject(GetObjectMngr().GetObj("ground"));
}

SsaoPrePass::~SsaoPrePass()
{

}

void SsaoPrePass::Init(ID3D12Device* _device) 
{
    device = _device;
	GetGlobalParam().GetSsao()->SetPSOs(
		GetRenderRsrcMngr().GetPSOMngr()->GetPipelineState("Ssao"),
		GetRenderRsrcMngr().GetPSOMngr()->GetPipelineState("SsaoBlur"));
}

void SsaoPrePass::Tick()
{
	// Init Once
    static bool is_sm_init = false;
    if (!is_sm_init)
    {
		GetGlobalParam().GetSsao()->BuildDescriptors(
			pack.mDepthStencilBuffer,
			CD3DX12_CPU_DESCRIPTOR_HANDLE(GetRenderRsrcMngr().GetTexMngr()->GetTexAllocation().GetCpuHandle(
                GetRenderRsrcMngr().GetTexMngr()->GetSSAOIdxStart()
            )),
			CD3DX12_GPU_DESCRIPTOR_HANDLE(GetRenderRsrcMngr().GetTexMngr()->GetTexAllocation().GetGpuHandle(
                GetRenderRsrcMngr().GetTexMngr()->GetSSAOIdxStart()
            )),
			CD3DX12_CPU_DESCRIPTOR_HANDLE(pack.rtvHandle),
			GetRenderRsrcMngr().GetTexMngr()->GetTexAllocation().GetDescriptorSize(),
			pack.rtvDescriptorSize);
    }
    is_sm_init = true;

	static SsaoConstants ssaoCB;

	DirectX::XMMATRIX P = pack.p2camera->GetProj();

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	DirectX::XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	ssaoCB.Proj = GetGlobalParam().GetMainPassData().Proj;
	ssaoCB.InvProj = GetGlobalParam().GetMainPassData().InvProj;
	DirectX::XMStoreFloat4x4(&ssaoCB.ProjTex, DirectX::XMMatrixTranspose(P * T));

	GetGlobalParam().GetSsao()->GetOffsetVectors(ssaoCB.OffsetVectors);

	auto blurWeights = GetGlobalParam().GetSsao()->CalcGaussWeights(2.5f);
	ssaoCB.BlurWeights[0] = DirectX::XMFLOAT4(&blurWeights[0]);
	ssaoCB.BlurWeights[1] = DirectX::XMFLOAT4(&blurWeights[4]);
	ssaoCB.BlurWeights[2] = DirectX::XMFLOAT4(&blurWeights[8]);

	ssaoCB.InvRenderTargetSize = DirectX::XMFLOAT2(
        1.0f / GetGlobalParam().GetSsao()->SsaoMapWidth(), 
        1.0f / GetGlobalParam().GetSsao()->SsaoMapHeight());

	// Coordinates given in view space.
	ssaoCB.OcclusionRadius = 0.5f;
	ssaoCB.OcclusionFadeStart = 0.2f;
	ssaoCB.OcclusionFadeEnd = 1.0f;
	ssaoCB.SurfaceEpsilon = 0.05f;

    pack.currFrameResource->GetResource<std::shared_ptr<UploadBuffer<SsaoPrePass::SsaoConstants>>>(
        "SsaoCB")->CopyData(0, ssaoCB);
}
