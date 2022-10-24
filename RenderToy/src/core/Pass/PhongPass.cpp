#include <Pass/PhongPass.h>

using namespace Chen::RToy;

PhongPass::PhongPass(std::string name) : IPass(name) 
{

}

PhongPass::~PhongPass()
{

}

void PhongPass::Init(ID3D12Device* _device, ID3D12GraphicsCommandList* _cmdList)
{
    device = _device;
    cmdList = _cmdList;
}

void PhongPass::Tick()
{

}
