#include <CDX12/Resource/UploadBuffer.h>

#include <rapidjson/document.h> 
#include <rapidjson/filereadstream.h>

#include <RenderToy/RenderToy.h>

#include <LogicalComponent/LogicalComponent.h>
#include <RenderComponent/RenderComponent.h>

#include <Editor/Editor.h>
#include <AssetMngr/AssetMngr.h>
#include <ObjectMngr/BasicObject.h>

#include <Pass/render/PhongPass.h>
#include <Pass/logical/UpdatePass.h>
#include <Pass/logical/SsaoPrePass.h>

#include <Utility/Macro.h>

using namespace Chen;
using namespace Chen::CDX12;
using namespace Chen::RToy;

using namespace rapidjson;

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

	// GetRenderRsrcMngr().GetMeshMngr()->BuildOBJModelGeometryFromFile("../../../assets/models/obj/Marry.obj", "Marry");
	// GetRenderRsrcMngr().GetMeshMngr()->BuildTXTModelGeometryFromFile("../../../assets/models/txt/marry.txt", "Marry", "Marry", true, true);

	PreBuildTexAndMatFromJson();

	GetAssetMngr().Init();
	GetObjectMngr().Init();
	GetPropertyMngr().Init();

	GetEditor().Init(
		mhMainWnd, 
		mDevice.Get(),
		GetRenderRsrcMngr().GetTexMngr()->GetTexAllocation().GetDescriptorHeap(),
		GetRenderRsrcMngr().GetTexMngr()->GetTexAllocation().GetCpuHandle(15),
		GetRenderRsrcMngr().GetTexMngr()->GetTexAllocation().GetGpuHandle(15),
		mCmdQueue.Get());

	RegisterComponent("RenderComponent", std::make_unique<RenderComponent>());
	RegisterComponent("LogicalComponent", std::make_unique<LogicalComponent>());

	GetGlobalParam().Init(
		mDevice.Get(), 
		mCmdList.Get(), 
		mClientWidth, 
		mClientHeight,
		GetRenderComponent(),
		GetLogicalComponent());

	BuildShaders();  // before BuildPSOs();
	BuildPSOs();
	BuildFrameResource();

	GetRenderComponent()->Init(mDevice.Get());
	GetLogicalComponent()->Init(mDevice.Get());

    ThrowIfFailed(mCmdList->Close());
    mCmdQueue.Execute(mCmdList.Get());
    FlushCommandQueue();

	return true;
}

// **********************************************************

void RenderToy::BuildShaders()
{
	// Caution: Remember to Set the InputLayout of Shader, otherwise Exception Raised.

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
            "Textures", Shader::Property{ShaderVariableType::SRVDescriptorHeap, 0, 4, 50} /* t4 space0 */
        ),	
		std::make_pair<std::string, Shader::Property>(
			"CubeMap", Shader::Property{ShaderVariableType::SRVDescriptorHeap, 0, 0, 4} /* t0 t1 t2 t3 space0 */
		),
		std::make_pair<std::string, Shader::Property>(
            "Materials", Shader::Property{ShaderVariableType::StructuredBuffer, 1, 0, 168} /* t0 space1 */
        ),	
    };

	std::filesystem::path shaderPath
		= GetAssetMngr().GetShaderPath() / "Phong/PhongShading.hlsl";

	// with a rootsig built together
	GetRenderRsrcMngr().GetShaderMngr()->CreateShader(
		"IShader", 
		rootProperties, 
		AnsiToWString(shaderPath.string()).c_str(),
		AnsiToWString(shaderPath.string()).c_str());
	GetRenderRsrcMngr().GetShaderMngr()->GetShader("IShader")->mInputLayout = DefaultInputLayout;

	shaderPath = GetAssetMngr().GetShaderPath() / "Phong/Sky.hlsl";
	GetRenderRsrcMngr().GetShaderMngr()->CreateShader(
		"SkyShader", 
		rootProperties, 
		AnsiToWString(shaderPath.string()).c_str(),
		AnsiToWString(shaderPath.string()).c_str());
	GetRenderRsrcMngr().GetShaderMngr()->GetShader("SkyShader")->mInputLayout = DefaultInputLayout;
	GetRenderRsrcMngr().GetShaderMngr()->GetShader("SkyShader")->rasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	GetRenderRsrcMngr().GetShaderMngr()->GetShader("SkyShader")->depthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	// For Shadow Map
	shaderPath = GetAssetMngr().GetShaderPath() / "Phong/Shadows.hlsl";
	GetRenderRsrcMngr().GetShaderMngr()->CreateShader(
		"ShadowShader", 
		rootProperties, 
		AnsiToWString(shaderPath.string()).c_str(),
		AnsiToWString(shaderPath.string()).c_str());
	GetRenderRsrcMngr().GetShaderMngr()->GetShader("ShadowShader")->mInputLayout = DefaultInputLayout;

	// ********************************************************************************************************
	// For SSAO
	shaderPath = GetAssetMngr().GetShaderPath() / "Phong/DrawNormals.hlsl";
	GetRenderRsrcMngr().GetShaderMngr()->CreateShader(
		"DrawNormalsShader",
		rootProperties,
		AnsiToWString(shaderPath.string()).c_str(),
		AnsiToWString(shaderPath.string()).c_str());
	GetRenderRsrcMngr().GetShaderMngr()->GetShader("DrawNormalsShader")->mInputLayout = DefaultInputLayout;

	ComPtr<ID3D12RootSignature> mSsaoRootSignature = nullptr;
	ComPtr<ID3D12RootSignature> mSsaoBlurRootSignature = nullptr;

	CD3DX12_DESCRIPTOR_RANGE texTable0;
	texTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0, 0);

	CD3DX12_DESCRIPTOR_RANGE texTable1;
	texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2, 0);

	// Root parameter can be a table, root descriptor or root constants.
	CD3DX12_ROOT_PARAMETER slotRootParameter[4];

	// Perfomance TIP: Order from most frequent to least frequent.
	slotRootParameter[0].InitAsConstantBufferView(0);
	slotRootParameter[1].InitAsConstants(1, 1);
	slotRootParameter[2].InitAsDescriptorTable(1, &texTable0, D3D12_SHADER_VISIBILITY_PIXEL);
	slotRootParameter[3].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_PIXEL);

	auto staticSamplers = GlobalSamplers::GetSSAOSamplers();

	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(4, slotRootParameter,
		(UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(mDevice->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(mSsaoRootSignature.GetAddressOf())));

	ThrowIfFailed(mDevice->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(mSsaoBlurRootSignature.GetAddressOf())));

	std::vector<std::pair<std::string, Shader::Property>> ssaoRootProperties = {
		std::make_pair<std::string, Shader::Property>(
			"SsaoPassCB", Shader::Property{ShaderVariableType::ConstantBuffer, 0, 0, 1} /* b0 space0 */
		),
		std::make_pair<std::string, Shader::Property>(
			"HorizontalBlur", Shader::Property{ShaderVariableType::ConstantBuffer, 0, 1, 1} /* Don't use it */
		),
		std::make_pair<std::string, Shader::Property>(
			"NormalMapGpuSrv", Shader::Property{ShaderVariableType::SRVDescriptorHeap, 0, 0, 2} 
		),
		std::make_pair<std::string, Shader::Property>(
			"RandomVectorMapGpuSrv", Shader::Property{ShaderVariableType::SRVDescriptorHeap, 0, 2, 1} 
		),
	};

	shaderPath = GetAssetMngr().GetShaderPath() / "Phong/Ssao.hlsl";
	GetRenderRsrcMngr().GetShaderMngr()->CreateShader(
		"SsaoShader", 
		ssaoRootProperties,
		std::move(mSsaoRootSignature),
		AnsiToWString(shaderPath.string()).c_str(),
		AnsiToWString(shaderPath.string()).c_str());
	GetRenderRsrcMngr().GetShaderMngr()->GetShader("SsaoShader")->mInputLayout =
		std::vector<D3D12_INPUT_ELEMENT_DESC>();
	GetRenderRsrcMngr().GetShaderMngr()->GetShader("SsaoShader")->depthStencilState.DepthEnable = false;
	GetRenderRsrcMngr().GetShaderMngr()->GetShader("SsaoShader")->depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	auto ssao = GetRenderRsrcMngr().GetShaderMngr()->GetShader("SsaoShader");
	
	shaderPath = GetAssetMngr().GetShaderPath() / "Phong/SsaoBlur.hlsl";
	GetRenderRsrcMngr().GetShaderMngr()->CreateShader(
		"SsaoBlurShader",
		ssaoRootProperties,
		std::move(mSsaoBlurRootSignature),
		AnsiToWString(shaderPath.string()).c_str(),
		AnsiToWString(shaderPath.string()).c_str());
	GetRenderRsrcMngr().GetShaderMngr()->GetShader("SsaoBlurShader")->mInputLayout =
		std::vector<D3D12_INPUT_ELEMENT_DESC>();
	GetRenderRsrcMngr().GetShaderMngr()->GetShader("SsaoBlurShader")->depthStencilState.DepthEnable = false;
	GetRenderRsrcMngr().GetShaderMngr()->GetShader("SsaoBlurShader")->depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

	// ********************************************************************************************************
}

void RenderToy::BuildPSOs()
{
	GetRenderRsrcMngr().GetPSOMngr()->CreatePipelineState(
		"Transparent",
		mDevice.Get(),
		GetRenderRsrcMngr().GetShaderMngr()->GetShader("IShader"),
		1,
		mBackBufferFormat,
		mDepthStencilFormat,
		true);

	GetRenderRsrcMngr().GetShaderMngr()->GetShader("IShader")->depthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_EQUAL;
	GetRenderRsrcMngr().GetShaderMngr()->GetShader("IShader")->depthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
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

	GetRenderRsrcMngr().GetPSOMngr()->CreatePipelineState(
		"ShadowMap",
		mDevice.Get(),
		GetRenderRsrcMngr().GetShaderMngr()->GetShader("ShadowShader"),
		0,
		mBackBufferFormat,
		mDepthStencilFormat, false, true);

	GetRenderRsrcMngr().GetPSOMngr()->CreatePipelineState(
		"Ssao",
		mDevice.Get(),
		GetRenderRsrcMngr().GetShaderMngr()->GetShader("SsaoShader"),
		1,
		Ssao::AmbientMapFormat,
		DXGI_FORMAT_UNKNOWN);

	GetRenderRsrcMngr().GetPSOMngr()->CreatePipelineState(
		"SsaoBlur",
		mDevice.Get(),
		GetRenderRsrcMngr().GetShaderMngr()->GetShader("SsaoBlurShader"),
		1,
		Ssao::AmbientMapFormat,
		DXGI_FORMAT_UNKNOWN);

	GetRenderRsrcMngr().GetPSOMngr()->CreatePipelineState(
		"DrawNormals",
		mDevice.Get(),
		GetRenderRsrcMngr().GetShaderMngr()->GetShader("DrawNormalsShader"),
		1,
		Ssao::NormalMapFormat,
		mDepthStencilFormat);
}

void RenderToy::PreBuildTexAndMatFromJson()
{
	std::filesystem::path filePath =
		GetAssetMngr().GetRootPath() / "json/PreMatAndTex.json";

	FILE* fp;
	errno_t err = fopen_s(&fp, filePath.string().c_str(), "rb");
	if (err == 0) OutputDebugString(L"\n\nFile Open Error!!!\n\n");

	char readBuffer[65536];
	FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	Document document;
	document.ParseStream(is);

	assert(document.HasMember("textures"));
	assert(document.HasMember("materials"));

	const Value& textures = document["textures"];
	const Value& materials = document["materials"];

	assert(textures.IsArray());
	assert(materials.IsArray());

	std::string rootPath = document["rootPath"].GetString();

	for (SizeType idx = 0; idx < textures.Size(); ++idx)
	{
		std::string path = rootPath + textures[idx]["path"].GetString();
		GetRenderRsrcMngr().GetTexMngr()->CreateTextureFromFile(
			mDevice.Get(),
			mCmdQueue.Get(),
			AnsiToWString(path).c_str(),
			textures[idx]["name"].GetString(),
			TextureMngr::TexFileFormat(textures[idx]["format"].GetUint()),
			TextureDimension(textures[idx]["texDimension"].GetUint()));
	}

	GetRenderRsrcMngr().GetTexMngr()->SetCubeIndex(document["cubeIndex"].GetInt());
	GetRenderRsrcMngr().GetTexMngr()->SetSMIndex(document["smIndex"].GetInt());
	GetRenderRsrcMngr().GetTexMngr()->SetCGLutIndex(document["cgLutIndex"].GetInt());
	GetRenderRsrcMngr().GetTexMngr()->SetSSAOIdxStart(document["ssaoStartIndex"].GetInt());
	GetRenderRsrcMngr().GetTexMngr()->SetNullCubeIdx(document["ssaoStartIndex"].GetInt()+5);

	for (SizeType idx = 0; idx < materials.Size(); ++idx)
	{
		std::string name = materials[idx]["name"].GetString();
		std::string texName = materials[idx]["texName"].GetString();
		int nmapIndex = -1;
		if (!materials[idx]["normalName"].IsNull())
			nmapIndex = GetRenderRsrcMngr().GetTexMngr()->GetTextureIndex(
				materials[idx]["normalName"].GetString());

		GetRenderRsrcMngr().GetMatMngr()->CreateMaterial(
			name,
			GetRenderRsrcMngr().GetTexMngr()->GetTextureIndex(texName),
			XMFLOAT4(
				materials[idx]["diffuseAlbedo"]["x"].GetFloat(), 
				materials[idx]["diffuseAlbedo"]["y"].GetFloat(), 
				materials[idx]["diffuseAlbedo"]["z"].GetFloat(), 
				materials[idx]["diffuseAlbedo"]["w"].GetFloat()),
			XMFLOAT3(
				materials[idx]["fresnelR0"]["x"].GetFloat(), 
				materials[idx]["fresnelR0"]["y"].GetFloat(), 
				materials[idx]["fresnelR0"]["z"].GetFloat()),
			materials[idx]["roughness"].GetFloat(), nmapIndex);
	}

	fclose(fp);
}


// Build FrameResource and Register Needed Resource

void RenderToy::BuildFrameResource()
{
    for (int i = 0; i < gNumFrameResources; ++i)
    {
        mFrameResourceMngr->GetFrameResources()[i]->RegisterResource(
            "PassCB", std::move(std::make_shared<UploadBuffer<UpdatePass::PassConstants>>(
				mDevice.Get(), 
				6, 
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
            "SsaoCB", std::move(std::make_shared<UploadBuffer<SsaoPrePass::SsaoConstants>>(
				mDevice.Get(), 
				1, 
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

	GetEditor().SetWidthAndHeight(mClientWidth, mClientHeight);

	mCamera->SetLens(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	
	static bool is_first = true;
	if (!is_first)
	{
		GetGlobalParam().GetSsao()->OnResize(mClientWidth, mClientHeight);
		GetGlobalParam().GetSsao()->RebuildDescriptors(mDepthStencilBuffer.Get());
	}
	is_first = false;
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
	pack.mCmdList = mCmdList;
	// for shadowMap
	pack.shadowDsv = dsvCpuDH.GetCpuHandle(1);
	// for ssao
	pack.mDepthStencilBuffer = mDepthStencilBuffer.Get();
	pack.rtvHandle = rtvCpuDH.GetCpuHandle(SwapChainBufferCount);
	pack.rtvDescriptorSize = rtvCpuDH.GetDescriptorSize();

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
	// for shadowMap
	pack.shadowDsv = dsvCpuDH.GetCpuHandle(1);

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
	GetEditor().Tick();
	Populate(gt);
	Execute();
}

// *********************************************************
// TODO: move the logic of control to Editor

void RenderToy::OnKeyboardInput(const GameTimer& gt)
{
	const float dt = gt.DeltaTime();

	// adjust the camera position
	float step = (GetEditor().IsEnableMove()) ? 15.0f : 0.0f;

	if (GetAsyncKeyState('W') & 0x8000) mCamera->Walk(step * dt);

	if (GetAsyncKeyState('S') & 0x8000) mCamera->Walk(-step * dt);

	if (GetAsyncKeyState('A') & 0x8000) mCamera->Strafe(-step * dt);

	if (GetAsyncKeyState('D') & 0x8000) mCamera->Strafe(step * dt);

	mCamera->UpdateViewMatrix();
}

void RenderToy::OnMouseDown(WPARAM btnState, int x, int y)
{
	mLastMousePos.x = x;
	mLastMousePos.y = y;

	 if ((btnState & MK_LBUTTON) != 0) Pick(x, y);

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
		float dx = (GetEditor().IsEnableMove()) ?  XMConvertToRadians(0.25f * static_cast<float>(x - mLastMousePos.x)) : 0.0f;
		float dy = (GetEditor().IsEnableMove()) ?  XMConvertToRadians(0.25f * static_cast<float>(y - mLastMousePos.y)) : 0.0f;

		// adjust the camera orientation
		mCamera->Pitch(dy / mouseMoveSensitivity);
		mCamera->RotateY(dx / mouseMoveSensitivity);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT RenderToy::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// For ImGui
	if (ImGui_ImplWin32_WndProcHandler(mhMainWnd, msg, wParam, lParam))
		return true;

	return DX12App::MsgProc(hwnd, msg, wParam, lParam);
}

int RenderToy::Run()
{
	MSG msg = { 0 };

	mTimer.Reset();

	while (msg.message != WM_QUIT)
	{
		// If there are Window messages then process them.
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// Otherwise, do animation/game stuff.
		else
		{
			mTimer.Tick();

			if (!mAppPaused)
			{
				CalculateFrameStats();
				LogicTick(mTimer);
				RenderTick(mTimer);
				if (GetEditor().IsExit()) break;
			}
			else
			{
				Sleep(100);
			}
		}
	}
	return (int)msg.wParam;
}

void RenderToy::Pick(int sx, int sy)
{
	if (sx <= 320 || sx >= 1470) return;

	XMFLOAT4X4 P = mCamera->GetProj4x4f();

	// Compute picking ray in view space. (with Topleftx = 310.0f)
	//float vx = (+2.0f * sx / (mClientWidth/1.653f) - 1.0f - 620.0f / (mClientWidth/1.653f)) / P(0, 0);
	//float vy = (-2.0f * sy / (mClientHeight/1.335f) + 1.0f) / P(1, 1);

	float vx = (+2.0f * sx / mClientWidth - 1.0f) / P(0, 0);
	float vy = (-2.0f * sy / mClientHeight + 1.0f) / P(1, 1);

	// Ray definition in view space.
	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	XMMATRIX V = mCamera->GetView();
	XMMATRIX invView = XMMatrixInverse(get_rvalue_ptr(XMMatrixDeterminant(V)), V);

	if (GetEditor().GetPickedID() != -1)
		dynamic_cast<BasicObject*>(GetObjectMngr().GetObj(GetEditor().GetPickedID()))->SetInvisible();

	for (int idx = 2; idx <= GetObjectMngr().GetBiggestID(); ++idx)
	{
		if (GetObjectMngr().GetObj(idx) != nullptr)
		{
			if (dynamic_cast<BasicObject*>(GetObjectMngr().GetObj(idx))->IsVisible())
			{
				rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
				rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

				XMMATRIX trans = XMLoadFloat4x4(get_rvalue_ptr(GetTransformOfObjByID(idx)->GetTranslate()));
				XMMATRIX scale = XMLoadFloat4x4(get_rvalue_ptr(GetTransformOfObjByID(idx)->GetScale()));
				XMMATRIX rotation = XMLoadFloat4x4(get_rvalue_ptr(GetTransformOfObjByID(idx)->GetRotation()));

				XMMATRIX W = scale * rotation * trans;
				XMMATRIX invWorld = XMMatrixInverse(get_rvalue_ptr(XMMatrixDeterminant(W)), W);

				// Tranform ray to vi space of Mesh.
				XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

				rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
				rayDir = XMVector3TransformNormal(rayDir, toLocal);

				// Make the ray direction unit length for the intersection tests.
				rayDir = XMVector3Normalize(rayDir);

				float tmin = 0.001f;
				if (GetMeshOfObjByID(idx)->GetBoundingBox().Intersects(rayOrigin, rayDir, tmin))
				{
					if (GetEditor().GetPickedID() != -1)
						dynamic_cast<BasicObject*>(GetObjectMngr().GetObj(GetEditor().GetPickedID()))->SetVisible();
					GetEditor().SetPickedID(idx);
				}
			}
		}
	}
}
