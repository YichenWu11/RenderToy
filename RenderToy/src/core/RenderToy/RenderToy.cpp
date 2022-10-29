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

const int mouseMoveSensitivity = 2;

const int maxObjectsNum = 168;

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
	BuildTextures();
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
            "ObjTransformCB", Shader::Property{ShaderVariableType::ConstantBuffer, 0, 0, 1}
        ),
        std::make_pair<std::string, Shader::Property>(
            "PassCB", Shader::Property{ShaderVariableType::ConstantBuffer, 0, 1, 1}
        )
    };

	// with a rootsig built together
	GetRenderRsrcMngr().GetShaderMngr()->CreateShader(
		"IShader", 
		rootProperties, 
		L"..\\..\\..\\shaders\\color.hlsl",
		L"..\\..\\..\\shaders\\color.hlsl");

	GetRenderRsrcMngr().GetShaderMngr()->GetShader("IShader")->mInputLayout = DefaultInputLayout;
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
}

void RenderToy::BuildTextures()
{
	// auto& alloc = RenderResourceMngr::GetInstance().GetTexMngr()->GetTexAllocation();
	
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
				168,
				true)));
		mFrameResourceMngr->GetFrameResources()[i]->RegisterResource(
            "MatIndexCB", std::move(std::make_shared<UploadBuffer<Material::ID>>(
				mDevice.Get(), 
				168, 
				true)));
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
	LogicalComponent::ComPack pack;
	/*
	// Fill the ComPack of LogicalComponent
	*/
	pack.currFrameResource = mCurrFrameResource;
	pack.p2camera = mCamera.get();
	pack.p2timer  = &mTimer;
	pack.width = mClientWidth;
	pack.height = mClientHeight;

	GetLogicalComponent()->FillPack(pack);
}

void RenderToy::RenderFillPack()
{
	RenderComponent::ComPack pack;
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

	/*
		Do logical Tick
	*/
	LogicalFillPack();
	GetLogicalComponent()->Tick();

	if (mFrameResourceMngr->GetCurrentCpuFence() != 0) mFrameResourceMngr->BeginFrame(); // Begin Frame Here
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
	if (GetAsyncKeyState('W') & 0x8000) mCamera->Walk(8.0f * dt);

	if (GetAsyncKeyState('S') & 0x8000) mCamera->Walk(-8.0f * dt);

	if (GetAsyncKeyState('A') & 0x8000) mCamera->Strafe(-8.0f * dt);

	if (GetAsyncKeyState('D') & 0x8000) mCamera->Strafe(8.0f * dt);

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
