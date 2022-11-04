#pragma once

#include <CDX12/Common/DX12App.h>
#include <CDX12/FrameResource.h>
#include <CDX12/Common/Camera.h>
#include <IComponent.h>

using namespace Chen::CDX12;

namespace Chen::RToy {
    class RenderToy final : public DX12App
    {
    public:
        RenderToy(HINSTANCE hInstance);
        RenderToy(const RenderToy&) = delete;
        RenderToy& operator=(const RenderToy&) = delete;
        ~RenderToy();

        bool Initialize() override;

        void RegisterComponent(std::string name, std::unique_ptr<IComponent> component);

    private:
        void OnResize() override;
        void LogicTick(const GameTimer& gt) override;
        void RenderTick(const GameTimer& gt) override;

        // **********************************************************
        // KeyBoard and Mouse Control

        void OnMouseDown(WPARAM btnState, int x, int y) override;
        void OnMouseUp(WPARAM btnState, int x, int y) override;
        void OnMouseMove(WPARAM btnState, int x, int y) override;
        void OnKeyboardInput(const GameTimer& gt);

        // **********************************************************


        // **********************************************************
        // Build FrameResource and Register Needed Resource

        void PreBuildTexAndMatFromJson();

        void BuildPSOs();
        void BuildShaders();
        void BuildFrameResource();

        // **********************************************************
 
        void LogicalFillPack();
        void RenderFillPack();
        void Populate(const GameTimer& gt);  // Populate Command
        void Execute();                      // Submit and Execute Command

        // **********************************************************

        IComponent* GetRenderComponent() 
        {
            assert(mComponents.find("RenderComponent") != mComponents.end());
            return mComponents["RenderComponent"].get();
        }

        IComponent* GetLogicalComponent()
        {
            assert(mComponents.find("LogicalComponent") != mComponents.end());
            return mComponents["LogicalComponent"].get();
        }

        // **********************************************************

        std::vector<std::string>& GetComponentNameList() { return nameList; }

    private:
        std::vector<std::string> nameList;
        std::map<std::string, std::unique_ptr<IComponent>> mComponents;

        POINT mLastMousePos;
        std::unique_ptr<Camera> mCamera;

        std::vector<D3D12_INPUT_ELEMENT_DESC> DefaultInputLayout;
        FrameResource* mCurrFrameResource = nullptr;
    };
}
