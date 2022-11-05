#pragma once

#include <CDX12/DescripitorHeap/DescriptorHeapAllocation.h>
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#include <imgui_internal.h>
#include <filesystem>


namespace Chen::RToy::Editor {
    class Editor
    {
    public:
        static Editor& GetInstance()
        {
            static Editor instance;
            return instance;
        }

        Editor(const Editor&) = delete;
        Editor& operator=(const Editor&) = delete;

        void Init(HWND, ID3D12Device*, ID3D12DescriptorHeap*, D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE, ID3D12CommandQueue*);

        void CleanUp();

        void Tick();

        void TickLeftSideBar();
        void TickBottomBar();
        void TickRightSideBar();

        void ShowDirContent(std::filesystem::path);

        bool IsExit() { return isExit; }

        bool IsEnableMove() { return enableMove; }
        void ToggleEnableMove() { enableMove = !enableMove; }

        int GetPickedID() { return pickedID; }
        void SetPickedID(int idx) { pickedID = idx; }

    private:
        Editor() = default;
        ~Editor();

        bool isExit = false;
        bool enableMove = true;
        ImGuiWindowFlags window_flags = 0;
        int pickedID = -1;

        ID3D12CommandQueue* cmdQueue;
        ID3D12Device* device;
    };
}
