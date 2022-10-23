#pragma once

#include <CDX12/Common/DX12App.h>
#include <CDX12/Common/Camera.h>
#include "LogicalComponent/LogicalComponent.h"
#include "RenderComponent/RenderComponent.h"

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

        void RegisterComponent(std::string name, std::unique_ptr<IComponent>& component);

    private:
        void OnResize() override;
        void Update(const GameTimer& gt) override;
        void Draw(const GameTimer& gt) override;

        // **********************************************************
        // KeyBoard and Mouse Control

        void OnMouseDown(WPARAM btnState, int x, int y) override;
        void OnMouseUp(WPARAM btnState, int x, int y) override;
        void OnMouseMove(WPARAM btnState, int x, int y) override;
        void OnKeyboardInput(const GameTimer& gt);

        // **********************************************************


        // **********************************************************
        // Build FrameResource and Register Needed Resource

        void BuildFrameResource();

        // **********************************************************
 
        void Populate(const GameTimer& gt);  // Populate Command
        void Execute();                      // Submit and Execute Command

        std::vector<std::string>& GetComponentNameList() { return nameList; }

    private:
        std::map<std::string, std::unique_ptr<IComponent>> mComponents;
        std::vector<std::string> nameList;

        POINT mLastMousePos;
        Camera mCamera;
    };
}
