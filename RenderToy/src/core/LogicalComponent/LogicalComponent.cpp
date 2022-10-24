#include <LogicalComponent/LogicalComponent.h>
#include <Pass/logical/UpdatePass.h>
#include <Pass/logical/TransUpdatePass.h>

using namespace Chen::RToy;

LogicalComponent::LogicalComponent()
{
    AddPass(std::move(std::make_unique<UpdatePass>()));  
    AddPass(std::move(std::make_unique<TransUpdatePass>()));
}

LogicalComponent::~LogicalComponent()
{

}

void LogicalComponent::Init(ID3D12Device* _device, ID3D12GraphicsCommandList* _cmdList)
{
    mPasses[std::string("UpdatePass")]->Init(_device, _cmdList);
    mPasses[std::string("TransUpdatePass")]->Init(_device, _cmdList);
}

void LogicalComponent::Tick()
{
    mPasses[std::string("UpdatePass")]->Tick();
    mPasses[std::string("TransUpdatePass")]->Tick();
}
