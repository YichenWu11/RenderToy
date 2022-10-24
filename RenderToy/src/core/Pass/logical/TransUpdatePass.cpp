#include <Pass/logical/TransUpdatePass.h>

using namespace Chen::RToy;

TransUpdatePass::TransUpdatePass(std::string name) : IPass(name) 
{

}

TransUpdatePass::~TransUpdatePass()
{

}

void TransUpdatePass::Init(ID3D12Device* _device, ID3D12GraphicsCommandList* _cmdList)
{
    device = _device;
    cmdList = _cmdList;
}

void TransUpdatePass::Tick()
{
    for (auto &p2obj : mObjects)
    {
        // Update the transform property of all objects
        if (p2obj.second->GetProperty("Transform")->IsDirty())
        {
            auto objImpl = std::any_cast<Transform::Impl>(p2obj.second->GetProperty("Transform")->GetImpl());
            Transform::Impl new_impl(objImpl);
            pack.currFrameResource->GetResource<std::shared_ptr<Chen::CDX12::UploadBuffer<Transform::Impl>>>(
                "ObjTransformCB")->CopyData(p2obj.second->GetID(), new_impl);
            p2obj.second->GetProperty("Transform")->ClearOne();
        }
    }
}
