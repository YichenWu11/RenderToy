#include <RenderToy/RenderToy.h>

using namespace Chen;
using namespace Chen::CDX12;
using namespace Chen::RToy;

// *********************************************************
// Constant

const int mouseMoveSensity = 1;

const int maxObjectsNum = 168;

// *********************************************************


RenderToy::RenderToy(HINSTANCE hInstance) : DX12App(hInstance) {}

RenderToy::~RenderToy() {}

void RenderToy::RegisterComponent(std::string name, std::unique_ptr<IComponent>& component)
{
	if (mComponents.find(name) != mComponents.end()) 
	{
		mComponents[name] = std::move(component);
		nameList.push_back(name);
	}
}

bool RenderToy::Initialize() 
{
	if (!DX12App::Initialize())
		return false;

	// TODO: Init

	return true;
}

// **********************************************************
// Build FrameResource and Register Needed Resource

void RenderToy::BuildFrameResource()
{

}

// **********************************************************

void RenderToy::OnResize()
{

}

void RenderToy::LogicTick(const GameTimer& gt)
{

}

void RenderToy::Populate(const GameTimer& gt)
{

}

void RenderToy::Execute()
{

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
		mCamera.Pitch(dy / mouseMoveSensity);
		mCamera.RotateY(dx / mouseMoveSensity);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}
