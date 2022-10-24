#include <RenderComponent/RenderComponent.h>
#include <Pass/PhongPass.h>

using namespace Chen::RToy;

RenderComponent::RenderComponent()
{
    AddPass(std::move(std::make_unique<PhongPass>()));
}

void RenderComponent::Init(ID3D12Device* _device, ID3D12GraphicsCommandList* _cmdList)
{
    mPasses[std::string("PhongPass")]->Init(_device, _cmdList);
}

void RenderComponent::Tick()
{
    
}

RenderComponent::~RenderComponent()
{

}
