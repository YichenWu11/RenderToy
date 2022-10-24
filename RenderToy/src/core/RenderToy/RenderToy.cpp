#include <RenderToy/RenderToy.h>
#include <ObjectMngr/ObjectMngr.h>
#include <ObjectMngr/BasicObject.h>
#include <CDX12/Resource/UploadBuffer.h>
#include <Pass/render/PhongPass.h>
#include <Pass/logical/UpdatePass.h>

using namespace Chen;
using namespace Chen::CDX12;
using namespace Chen::RToy;

// *********************************************************
// Constant

const int mouseMoveSensitivity = 1;

const int maxObjectsNum = 168;

const int gNumFrameResources = 3;

// *********************************************************


RenderToy::RenderToy(HINSTANCE hInstance) : DX12App(hInstance) 
{
	mMainWndCaption = L"RenderToy";
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

	// TODO: Init
	RegisterComponent("RenderComponent", std::make_unique<RenderComponent>());
	RegisterComponent("LogicalComponent", std::make_unique<LogicalComponent>());

	RenderResourceMngr::GetInstance().Init(mDevice.Get(), mCmdList.Get());

	BuildShaders();
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
            "ObjectCB", Shader::Property{ShaderVariableType::ConstantBuffer, 0, 0, 1}
        ),
        std::make_pair<std::string, Shader::Property>(
            "PassCB", Shader::Property{ShaderVariableType::ConstantBuffer, 0, 1, 1}
        )
    };

	// with a rootsig built
	RenderResourceMngr::GetInstance().GetShaderMngr()->CreateShader(
		"IShader", 
		rootProperties, 
		L"..\\..\\shaders\\color.hlsl",
		L"..\\..\\shaders\\color.hlsl");

	RenderResourceMngr::GetInstance().GetShaderMngr()->GetShader("IShader")->mInputLayout = DefaultInputLayout;
}

void RenderToy::BuildPSOs()
{
	RenderResourceMngr::GetInstance().GetPSOMngr()->CreatePipelineState(
		"Base", 
		mDevice.Get(),
		RenderResourceMngr::GetInstance().GetShaderMngr()->GetShader("IShader"),
		1,
		mBackBufferFormat,
		mDepthStencilFormat);
}

void RenderToy::BuildTexture()
{
	// auto& alloc = RenderResourceMngr::GetInstance().GetTexMngr()->GetTexAllocation();
	
}

void RenderToy::BuildObjects()
{
	ObjectMngr::GetInstance().AddObject(std::make_shared<BasicObject>("box1"));
	GetRenderComponent()->GetPass("PhongPass")->AddObject(ObjectMngr::GetInstance().GetObj(1));
	
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
				1, //(UINT)ObjectMngr::GetInstance().GetObjNum() 
				true)));
    }	
}

// **********************************************************

void RenderToy::OnResize()
{
	DX12App::OnResize();

	mCamera.SetLens(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
}

void RenderToy::LogicalFillPack()
{

}

void RenderToy::RenderFillPack()
{

}

void RenderToy::LogicTick(const GameTimer& gt)
{
    OnKeyboardInput(gt);

    mCurrFrameResource = mFrameResourceMngr->GetCurrentFrameResource();

	/*
		Do logical Tick
	*/
	LogicalFillPack();
	GetRenderComponent()->Tick();

	if (mFrameResourceMngr->GetCurrentCpuFence() != 0) mFrameResourceMngr->BeginFrame(); // Begin Frame Here
}

void RenderToy::Populate(const GameTimer& gt)
{
	CmdListHandle cmdListHandle = { mCurrFrameResource->GetAllocator(), mCmdList.Get() };
	RenderFillPack();
	GetRenderComponent()->Tick();
}

void RenderToy::Execute()
{
	mCmdQueue.Execute(mCmdList.Get());
    ThrowIfFailed(mSwapChain->Present(0, 0));
    mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;
    mFrameResourceMngr->EndFrame(mCmdQueue.Get());  // // End Frame Here
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
	if (GetAsyncKeyState('W') & 0x8000) mCamera.Walk(10.0f * dt);

	if (GetAsyncKeyState('S') & 0x8000) mCamera.Walk(-10.0f * dt);

	if (GetAsyncKeyState('A') & 0x8000) mCamera.Strafe(-10.0f * dt);

	if (GetAsyncKeyState('D') & 0x8000) mCamera.Strafe(10.0f * dt);

	mCamera.UpdateViewMatrix();
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
		mCamera.Pitch(dy / mouseMoveSensitivity);
		mCamera.RotateY(dx / mouseMoveSensitivity);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}
