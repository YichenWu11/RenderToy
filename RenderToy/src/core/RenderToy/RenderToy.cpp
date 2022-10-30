#include <RenderToy/RenderToy.h>
#include <Editor/Editor.h>
#include <AssetMngr/AssetMngr.h>
#include <ObjectMngr/BasicObject.h>
#include <CDX12/Resource/UploadBuffer.h>
#include <Pass/render/PhongPass.h>
#include <Pass/logical/UpdatePass.h>
#include <Utility/Macro.h>

using namespace Chen;
using namespace Chen::CDX12;
using namespace Chen::RToy;

// *********************************************************
// Constant

#define GetEditor()    Chen::RToy::Editor::Editor::GetInstance()
#define GetAssetMngr() Chen::RToy::Asset::AssetMngr::GetInstance()

const float mouseMoveSensitivity = 1.0;

const UINT maxObjectsNum = 168;

const UINT maxMaterialNum = 168;

const int gNumFrameResources = 3;

// *********************************************************


RenderToy::RenderToy(HINSTANCE hInstance) : DX12App(hInstance) 
{
	mMainWndCaption = L"RenderToy";
	mCamera = std::make_unique<Camera>();
	DefaultInputLayout = 
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};
}

RenderToy::~RenderToy() {}

void RenderToy::RegisterComponent(std::string name, std::unique_ptr<IComponent> component)
{
	if (mComponents.find(name) == mComponents.end()) 
	{
		mComponents[name] = std::move(component);
		nameList.push_back(name);
	}
}

bool RenderToy::Initialize() 
{
	if (!DX12App::Initialize())
		return false;

	mCmdList.Reset(mDirectCmdListAlloc.Get());

	// set camera position
	mCamera->SetPosition(0.0f, 4.0f, -15.0f);

	/*
		Init
	*/
	GetRenderRsrcMngr().Init(mDevice.Get(), mCmdList.Get());

	BuildTextures();
	BuildMaterials();

	GetObjectMngr().Init();
	GetPropertyMngr().Init();
	GetEditor().Init();
	GetAssetMngr().Init();

	RegisterComponent("RenderComponent", std::make_unique<RenderComponent>());
	RegisterComponent("LogicalComponent", std::make_unique<LogicalComponent>());

	GetRenderComponent()->Init(mDevice.Get());
	GetLogicalComponent()->Init(mDevice.Get());

	BuildShaders();  // before BuildPSOs();
	BuildPSOs();
	BuildFrameResource();

    ThrowIfFailed(mCmdList->Close());
    mCmdQueue.Execute(mCmdList.Get());
    FlushCommandQueue();

	return true;
}

// **********************************************************

void RenderToy::BuildShaders()
{
	std::vector<std::pair<std::string, Shader::Property>> rootProperties = {
        std::make_pair<std::string, Shader::Property>(
            "ObjTransformCB", Shader::Property{ShaderVariableType::ConstantBuffer, 0, 0, 1} /* b0 space0 */
        ),
		std::make_pair<std::string, Shader::Property>(
            "MatIndexCB", Shader::Property{ShaderVariableType::ConstantBuffer, 1, 0, 1} /* b0 space1 */
        ),
        std::make_pair<std::string, Shader::Property>(
            "PassCB", Shader::Property{ShaderVariableType::ConstantBuffer, 0, 1, 1} /* b1 space0 */
        ),
		std::make_pair<std::string, Shader::Property>(
            "Textures", Shader::Property{ShaderVariableType::SRVDescriptorHeap, 0, 1, 50} /* t1 space0 */
        ),	
		std::make_pair<std::string, Shader::Property>(
			"CubeMap", Shader::Property{ShaderVariableType::SRVDescriptorHeap, 0, 0, 1} /* t1 space0 */
		),
		std::make_pair<std::string, Shader::Property>(
            "Materials", Shader::Property{ShaderVariableType::StructuredBuffer, 1, 0, 168} /* t0 space1 */
        ),	
    };

	// with a rootsig built together
	GetRenderRsrcMngr().GetShaderMngr()->CreateShader(
		"IShader", 
		rootProperties, 
		L"..\\..\\shaders\\Phong\\PhongShading.hlsl",
		L"..\\..\\shaders\\Phong\\PhongShading.hlsl");

	GetRenderRsrcMngr().GetShaderMngr()->GetShader("IShader")->mInputLayout = DefaultInputLayout;

	GetRenderRsrcMngr().GetShaderMngr()->CreateShader(
		"SkyShader", 
		rootProperties, 
		L"..\\..\\shaders\\Phong\\Sky.hlsl",
		L"..\\..\\shaders\\Phong\\Sky.hlsl");

	GetRenderRsrcMngr().GetShaderMngr()->GetShader("SkyShader")->mInputLayout = DefaultInputLayout;
	GetRenderRsrcMngr().GetShaderMngr()->GetShader("SkyShader")->rasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	GetRenderRsrcMngr().GetShaderMngr()->GetShader("SkyShader")->depthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

void RenderToy::BuildPSOs()
{
	GetRenderRsrcMngr().GetPSOMngr()->CreatePipelineState(
		"Base", 
		mDevice.Get(),
		GetRenderRsrcMngr().GetShaderMngr()->GetShader("IShader"),
		1,
		mBackBufferFormat,
		mDepthStencilFormat);

	GetRenderRsrcMngr().GetPSOMngr()->CreatePipelineState(
		"Sky", 
		mDevice.Get(),
		GetRenderRsrcMngr().GetShaderMngr()->GetShader("SkyShader"),
		1,
		mBackBufferFormat,
		mDepthStencilFormat);
}

void RenderToy::BuildTextures()
{
	GetRenderRsrcMngr().GetTexMngr()->CreateTextureFromFile(
		mDevice.Get(),
		mCmdQueue.Get(),
		L"..\\..\\assets\\texture\\common\\bricks.dds",
		"bricks",
		TextureMngr::TexFileFormat::DDS);
	GetRenderRsrcMngr().GetTexMngr()->CreateTextureFromFile(
		mDevice.Get(),
		mCmdQueue.Get(),
		L"..\\..\\assets\\texture\\common\\bricks2.dds",
		"bricks2",
		TextureMngr::TexFileFormat::DDS);
	GetRenderRsrcMngr().GetTexMngr()->CreateTextureFromFile(
		mDevice.Get(),
		mCmdQueue.Get(),
		L"..\\..\\assets\\texture\\common\\bricks2_nmap.dds",
		"bricks_nmap",
		TextureMngr::TexFileFormat::DDS);
	GetRenderRsrcMngr().GetTexMngr()->CreateTextureFromFile(
		mDevice.Get(),
		mCmdQueue.Get(),
		L"..\\..\\assets\\texture\\common\\tile_nmap.dds",
		"tile_nmap",
		TextureMngr::TexFileFormat::DDS);
	GetRenderRsrcMngr().GetTexMngr()->CreateTextureFromFile(
		mDevice.Get(),
		mCmdQueue.Get(),
		L"..\\..\\assets\\texture\\common\\checkboard.dds",
		"checkboard",
		TextureMngr::TexFileFormat::DDS);

	GetRenderRsrcMngr().GetTexMngr()->SetCubeIndex(
		GetRenderRsrcMngr().GetTexMngr()->CreateTextureFromFile(
			mDevice.Get(),
			mCmdQueue.Get(),
			L"..\\..\\assets\\texture\\sky\\snowcube1024.dds",
			"cubeMap",
			TextureMngr::TexFileFormat::DDS,
			TextureDimension::Cubemap)
	);
}

void RenderToy::BuildMaterials()
{
	GetRenderRsrcMngr().GetMatMngr()->CreateMaterial(
		"bricks",
		GetRenderRsrcMngr().GetTexMngr()->GetTextureIndex("bricks"),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT3(0.2f, 0.2f, 0.2f),
		0.8f,
		GetRenderRsrcMngr().GetTexMngr()->GetTextureIndex("bricks_nmap"));

	GetRenderRsrcMngr().GetMatMngr()->CreateMaterial(
		"matForSphere",
		GetRenderRsrcMngr().GetTexMngr()->GetTextureIndex("bricks2"),
		XMFLOAT4(1.0f, 0.9f, 0.9f, 1.0f),
		XMFLOAT3(0.1f, 0.1f, 0.1f),
		0.8f,
		GetRenderRsrcMngr().GetTexMngr()->GetTextureIndex("bricks_nmap"));

	GetRenderRsrcMngr().GetMatMngr()->CreateMaterial(
		"bricksForGround",
		GetRenderRsrcMngr().GetTexMngr()->GetTextureIndex("checkboard"),
		XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f),
		XMFLOAT3(0.2f, 0.2f, 0.2f),
		0.8f,
		GetRenderRsrcMngr().GetTexMngr()->GetTextureIndex("tile_nmap"));
	GetRenderRsrcMngr().GetMatMngr()->CreateMaterial(
		"sky",
		GetRenderRsrcMngr().GetTexMngr()->GetTextureIndex("cubeMap"),
		XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		XMFLOAT3(0.1f, 0.1f, 0.1f),
		0.8f);
}

// Build FrameResource and Register Needed Resource

void RenderToy::BuildFrameResource()
{
    for (int i = 0; i < gNumFrameResources; ++i)
    {
        mFrameResourceMngr->GetFrameResources()[i]->RegisterResource(
            "PassCB", std::move(std::make_shared<UploadBuffer<UpdatePass::PassConstants>>(
				mDevice.Get(), 
				1, 
				true)));
        mFrameResourceMngr->GetFrameResources()[i]->RegisterResource(
            "ObjTransformCB", std::move(std::make_shared<UploadBuffer<Transform::Impl>>(
				mDevice.Get(), 
				maxObjectsNum,
				true)));
		mFrameResourceMngr->GetFrameResources()[i]->RegisterResource(
            "MatIndexCB", std::move(std::make_shared<UploadBuffer<Material::ID>>(
				mDevice.Get(), 
				maxObjectsNum,
				true)));
		mFrameResourceMngr->GetFrameResources()[i]->RegisterResource(
            "MaterialData", std::move(std::make_shared<UploadBuffer<BasicMaterialData>>(
				mDevice.Get(), 
				maxMaterialNum, 
				false)));
    }	
}

// **********************************************************

void RenderToy::OnResize()
{
	DX12App::OnResize();

	mCamera->SetLens(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
}

void RenderToy::LogicalFillPack()
{
	static LogicalComponent::ComPack pack;
	/*
	// Fill the ComPack of LogicalComponent
	*/
	pack.currFrameResource = mCurrFrameResource;
	pack.p2camera = mCamera.get();
	pack.p2timer = &mTimer;
	pack.width = mClientWidth;
	pack.height = mClientHeight;

	GetLogicalComponent()->FillPack(pack);
}

void RenderToy::RenderFillPack()
{
	static RenderComponent::ComPack pack;
	/*
	// Fill the ComPack of RenderComponent
	*/
	pack.mCmdList = mCmdList;
	pack.currFrameResource = mCurrFrameResource;
	pack.currBackBuffer = CurrentBackBuffer();
	pack.currBackBufferView = CurrentBackBufferView();
	pack.depthStencilView = DepthStencilView();
	pack.mScissorRect = mScissorRect;
	pack.mScreenViewport = mScreenViewport;

	GetRenderComponent()->FillPack(pack);
}

void RenderToy::LogicTick(const GameTimer& gt)
{
    OnKeyboardInput(gt);

    mCurrFrameResource = mFrameResourceMngr->GetCurrentFrameResource();

	if (mFrameResourceMngr->GetCurrentCpuFence() != 0) mFrameResourceMngr->BeginFrame(); // Begin Frame Here

	/*
		Do logical Tick
	*/
	LogicalFillPack();
	GetLogicalComponent()->Tick();
}

void RenderToy::Populate(const GameTimer& gt)
{
	CmdListHandle cmdListHandle = { mCurrFrameResource->GetAllocator(), mCmdList.Get() };

	/*
		Do Render Tick
	*/
	RenderFillPack();
	GetRenderComponent()->Tick();
}

void RenderToy::Execute()
{
	mCmdQueue.Execute(mCmdList.Get());
    ThrowIfFailed(mSwapChain->Present(0, 0));
    mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
    mFrameResourceMngr->EndFrame(mCmdQueue.Get());  // End Frame Here
}

void RenderToy::RenderTick(const GameTimer& gt)
{
	Populate(gt);
	Execute();
}

// *********************************************************
// TODO: move the logic of control to Editor

void RenderToy::OnKeyboardInput(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	// adjust the camera position
	if (GetAsyncKeyState('W') & 0x8000) mCamera->Walk(15.0f * dt);

	if (GetAsyncKeyState('S') & 0x8000) mCamera->Walk(-15.0f * dt);

	if (GetAsyncKeyState('A') & 0x8000) mCamera->Strafe(-15.0f * dt);

	if (GetAsyncKeyState('D') & 0x8000) mCamera->Strafe(15.0f * dt);

	mCamera->UpdateViewMatrix();
}

void RenderToy::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	// if ((btnState & MK_LBUTTON) != 0) Pick(x, y);

	SetCapture(mhMainWnd);
}

void RenderToy::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void RenderToy::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y));

		// adjust the camera orientation
		mCamera->Pitch(dy / mouseMoveSensitivity);
		mCamera->RotateY(dx / mouseMoveSensitivity);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}
