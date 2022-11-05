#include <Editor/Editor.h>
#include <Utility/Macro.h>
#include <AssetMngr/AssetMngr.h>

using namespace std::filesystem;
using namespace DirectX;

using namespace Chen;
using namespace Chen::RToy::Editor;

void Editor::Init(
	HWND mhMainWnd, 
	ID3D12Device* device,
	ID3D12DescriptorHeap* heap,
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle,
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
{
    IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGuiStyle& style = ImGui::GetStyle();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	style.FrameRounding = 8.0f;
	style.WindowPadding = ImVec2(8.0, 2.0);
	style.FramePadding = ImVec2(8.0, 2.0);
	ImGui::StyleColorsDark();
    
	io.Fonts->AddFontFromFileTTF("../../assets/resource/PilotEditorFont.TTF",
		22,
		nullptr,
		nullptr);
	io.Fonts->Build();

	ImGui_ImplWin32_Init(mhMainWnd);
	ImGui_ImplDX12_Init(device, 3,
		DXGI_FORMAT_R8G8B8A8_UNORM, heap,
		cpuHandle,
		gpuHandle);

	ImVec4* colors = style.Colors;
	colors[ImGuiCol_Text] = ImVec4(0.792f, 0.792f, 0.792f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.109f, 0.109f, 0.109f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.109f, 0.109f, 0.109f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.21f, 0.21f, 0.21f, 0.5411f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.294f, 0.294f, 0.294f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.294f, 0.294f, 0.294f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.297f, 0.156f, 0.25f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.397f, 0.156f, 0.25f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.50f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(93.0f / 255.0f, 10.0f / 255.0f, 66.0f / 255.0f, 1.00f);
	colors[ImGuiCol_SliderGrab] = colors[ImGuiCol_CheckMark];
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.3647f, 0.0392f, 0.2588f, 0.50f);
	colors[ImGuiCol_Button] = ImVec4(0.0117f, 0.0117f, 0.0117f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.5235f, 0.0235f, 0.0235f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.0353f, 0.0196f, 0.0235f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.1137f, 0.0235f, 0.0745f, 0.588f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(5.0f / 255.0f, 5.0f / 255.0f, 5.0f / 255.0f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.0f, 0.0f, 0.0f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(45.0f / 255.0f, 7.0f / 255.0f, 26.0f / 255.0f, 1.00f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(45.0f / 255.0f, 7.0f / 255.0f, 26.0f / 255.0f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.20f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	colors[ImGuiCol_Tab] = ImVec4(6.0f / 255.0f, 6.0f / 255.0f, 8.0f / 255.0f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(45.0f / 255.0f, 7.0f / 255.0f, 26.0f / 255.0f, 150.0f / 255.0f);
	colors[ImGuiCol_TabActive] = ImVec4(47.0f / 255.0f, 6.0f / 255.0f, 29.0f / 255.0f, 1.0f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(45.0f / 255.0f, 7.0f / 255.0f, 26.0f / 255.0f, 25.0f / 255.0f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(6.0f / 255.0f, 6.0f / 255.0f, 8.0f / 255.0f, 200.0f / 255.0f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(2.0f / 255.0f, 2.0f / 255.0f, 2.0f / 255.0f, 1.0f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

	//window_flags |= ImGuiWindowFlags_NoTitleBar;
	//window_flags |= ImGuiWindowFlags_NoScrollbar;
	//window_flags |= ImGuiWindowFlags_MenuBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	//window_flags |= ImGuiWindowFlags_NoCollapse;
	//window_flags |= ImGuiWindowFlags_NoNav;
	//window_flags |= ImGuiWindowFlags_NoBackground;
	//window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	//window_flags |= ImGuiWindowFlags_UnsavedDocument;
}

void Editor::CleanUp()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Editor::TickLeftSideBar()
{
	ImGui::SetNextWindowBgAlpha(1.0f);
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("Menu"))
	{
		if (ImGui::MenuItem("Export Scene"))
		{

		}
		if (ImGui::MenuItem("Load Scene")) 
		{

		}
		if (ImGui::MenuItem("Exit"))
		{
			isExit = true;
		}
		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();

	{
		ImGui::Begin("Materials And Objects", NULL, window_flags);

		//ImGui::ShowDemoWindow();

		ImGui::SetWindowPos(ImVec2(0, 23), ImGuiCond_Always);
		ImGui::SetWindowSize(ImVec2(405, 665));

		// Materials
		if (ImGui::CollapsingHeader("Materials")) {
			const std::vector<std::string>& matNameList = GetRenderRsrcMngr().GetMatMngr()->GetMatNameList();
			for (auto& name : matNameList)
			{
				if (ImGui::Selectable(name.c_str()))
				{

				}
			}
			ImGui::Button("Edit Materials");
		}

		// Objects
		if (ImGui::CollapsingHeader("Objects")) {
			const std::set<std::string>& objNameList = GetObjectMngr().GetNameList();
			for (auto& name : objNameList)
			{
				if (ImGui::Selectable(name.c_str()))
				{
					SetPickedID(GetObjectMngr().GetObj(name)->GetID());
				}
			}
			ImGui::Button("Add New Object");
			ImGui::Button("Delete Picked Object");
		}

		ImGui::End();
	}
}

void Editor::TickBottomBar()
{
	{
		ImGui::Begin("Files", NULL, window_flags);

		ImGui::SetWindowPos(ImVec2(0, 688), ImGuiCond_Always);
		ImGui::SetWindowSize(ImVec2(1471, 342));

		if (ImGui::CollapsingHeader("Assets"))
		{
			ShowDirContent(Asset::AssetMngr::GetInstance().GetRootPath());
		}

		ImGui::End();
	}
}

void Editor::ShowDirContent(std::filesystem::path path)
{
	directory_iterator list(path);
	for (auto& it : list) {
		if (it.status().type() == file_type::directory)
		{
			std::string dirName = it.path().filename().string();
			if (ImGui::TreeNode(dirName.c_str()))
			{
				ShowDirContent(it.path());
				ImGui::TreePop();
			}
		}
		else
		{
			std::string name = "    " + it.path().filename().string();
			if (ImGui::Selectable(name.c_str()))
			{

			}
		}
	}
}

void Editor::TickRightSideBar()
{
	{
		ImGui::Begin("Properties Details", NULL, window_flags);

		ImGui::SetWindowPos(ImVec2(1471, 23), ImGuiCond_Always);
		ImGui::SetWindowSize(ImVec2(450, 992));

		{
			if (pickedID != -1)
			{
				if (ImGui::CollapsingHeader("[Mesh]"))
				{
					static std::string geoName = GetMeshOfObjByID(pickedID)->GetMeshGeo()->Name;
					static std::string meshName = GetMeshOfObjByID(pickedID)->GetSubName();
					static const char* geo = { geoName.c_str() };
					static const char* mesh = { meshName.c_str() };
					static int currGID = 0;
					static int currMID = 0;

					if (ImGui::TreeNode("Geo"))
					{
						ImGui::Combo(" ", &currGID, geo, IM_ARRAYSIZE(geo));
						ImGui::TreePop();
					}

					if (ImGui::TreeNode("Mesh"))
					{
						ImGui::Combo(" ", &currMID, mesh, IM_ARRAYSIZE(mesh));
						ImGui::TreePop();
					}
				}

				if (ImGui::CollapsingHeader("[Transform]"))
				{
					static DirectX::XMFLOAT4X4 r;
					static DirectX::XMFLOAT4X4 s;
					static DirectX::XMFLOAT4X4 t;

					static DirectX::XMFLOAT3 scale(1.0f, 1.0f, 1.0f);
					static DirectX::XMFLOAT3 trans(0.0f, 0.0f, 0.0f);

					static DirectX::XMFLOAT3 axis(0.0f, 1.0f, 0.0f);
					static float angle = 0.0f;

					if (ImGui::TreeNode("Rotation"))
					{
						ImGui::DragFloat("AXIS_X", &axis.x, 0.1f, -1.0f, 1.0f, "%.2f");
						ImGui::DragFloat("AXIS_Y", &axis.y, 0.1f, -1.0f, 1.0f, "%.2f");
						ImGui::DragFloat("AXIS_Z", &axis.z, 0.1f, -1.0f, 1.0f, "%.2f");
						ImGui::DragFloat("ANGLE", &angle, 0.1f, 0.0f, 360.0f, "%.2f");

						XMMATRIX R = DirectX::XMMatrixRotationQuaternion(
							Math::Quaternion(DirectX::XMLoadFloat3(&axis), angle));

						XMStoreFloat4x4(&r, R);
						GetTransformOfObjByID(pickedID)->SetRotation(r);

						ImGui::TreePop();
					}

					if (ImGui::TreeNode("Scale"))
					{
						scale.x = GetTransformOfObjByID(pickedID)->GetScale()._11;
						scale.y = GetTransformOfObjByID(pickedID)->GetScale()._22;
						scale.z = GetTransformOfObjByID(pickedID)->GetScale()._33;

						ImGui::DragFloat("X", &scale.x, 0.1f, 0.1f, 50.0f, "%.2f");
						ImGui::DragFloat("Y", &scale.y, 0.1f, 0.1f, 50.0f, "%.2f");
						ImGui::DragFloat("Z", &scale.z, 0.1f, 0.1f, 50.0f, "%.2f");

						XMStoreFloat4x4(&s, XMMatrixScaling(scale.x, scale.y, scale.z));
						GetTransformOfObjByID(pickedID)->SetScale(s);

						ImGui::TreePop();
					}

					if (ImGui::TreeNode("Translate"))
					{
						trans.x = GetTransformOfObjByID(pickedID)->GetTranslate()._41;
						trans.y = GetTransformOfObjByID(pickedID)->GetTranslate()._42;
						trans.z = GetTransformOfObjByID(pickedID)->GetTranslate()._43;

						ImGui::DragFloat("X", &trans.x, 0.1f, 0.0f, 0.0f, "%.2f");
						ImGui::DragFloat("Y", &trans.y, 0.1f, 0.0f, 0.0f, "%.2f");
						ImGui::DragFloat("Z", &trans.z, 0.1f, 0.0f, 0.0f, "%.2f");

						XMStoreFloat4x4(&t, XMMatrixTranslation(trans.x, trans.y, trans.z));
						GetTransformOfObjByID(pickedID)->SetTranslate(t);

						ImGui::TreePop();
					}
				}

				if (ImGui::CollapsingHeader("[Material]"))
				{
					static Material* mat = GetMaterialOfObjByID(pickedID);

					if (ImGui::TreeNode("Name"))
					{
						ImGui::Text(mat->GetMatName().c_str());
						ImGui::TreePop();
					}

					if (ImGui::TreeNode("Diffuse TexName"))
					{
						static std::string diffName = GetRenderRsrcMngr().GetTexMngr()->GetTexNameFromID(
							mat->GetDiffuse());

						ImGui::Text(diffName.c_str());
						ImGui::TreePop();
					}

					if (ImGui::TreeNode("Normal TexName"))
					{
						static std::string normalName = GetRenderRsrcMngr().GetTexMngr()->GetTexNameFromID(
							mat->GetNormal());

						ImGui::Text(normalName.c_str());
						ImGui::TreePop();
					}

					if (ImGui::TreeNode("DiffuseAlbedo"))
					{
						static XMFLOAT4 albedo = mat->GetAlbedo();
						ImGui::Text("R: %.2f\nG: %.2f\nB: %.2f\nA: %.2f", albedo.x, albedo.y, albedo.z, albedo.w);
						ImGui::TreePop();
					}

					if (ImGui::TreeNode("FresnelR0"))
					{
						static XMFLOAT3 r0 = mat->GetFresnelR0();
						ImGui::Text("R: %.2f\nG: %.2f\nB: %.2f", r0.x, r0.y, r0.z);
						ImGui::TreePop();
					}

					if (ImGui::TreeNode("Roughness"))
					{
						ImGui::Text("Roughness: %.2f", mat->GetRoughness());
						ImGui::TreePop();
					}

					if (ImGui::TreeNode("MatTransform"))
					{
						static XMFLOAT4X4 mT = mat->GetMatTransform();
						ImGui::Text("X: %.2f\nY: %.2f\nZ: %.2f", mT._11, mT._22, mT._33);
						ImGui::TreePop();
					}

				}
			}
		}

		ImGui::End();
	}
}

void Editor::Tick()
{
	// TODO: Adaptive width and height

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	TickLeftSideBar();
	TickBottomBar();
	TickRightSideBar();

	ImGui::Render();
}

Editor::~Editor()
{
	CleanUp();
}