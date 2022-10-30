#include <Pass/logical/TransUpdatePass.h>
#include <PropertyMngr/Transform.h>
#include <DirectXMath.h>
#include <Utility/Macro.h>

using namespace Chen::RToy;

TransUpdatePass::TransUpdatePass(std::string name) : IPass(name) 
{
    AddObject(GetObjectMngr().GetObj("box1"));
    AddObject(GetObjectMngr().GetObj("sphere1"));
    AddObject(GetObjectMngr().GetObj("ground"));
}

TransUpdatePass::~TransUpdatePass()
{

}

void TransUpdatePass::Init(ID3D12Device* _device)
{
    device = _device;
}

void TransUpdatePass::Tick()
{
    for (auto &p2obj : mObjects)
    {
        // Update the transform property of all objects
        if (p2obj.second->GetProperty("Transform")->IsDirty())
        {
            auto objImpl = p2obj.second->GetPropertyImpl<Transform>("Transform");
            Transform::Impl new_impl(objImpl);

            // transpose
            DirectX::XMMATRIX trans = DirectX::XMLoadFloat4x4(&new_impl.Translate);
            DirectX::XMMATRIX rotate = DirectX::XMLoadFloat4x4(&new_impl.Rotation);
            DirectX::XMMATRIX scale = DirectX::XMLoadFloat4x4(&new_impl.Scale);

            DirectX::XMStoreFloat4x4(&new_impl.Translate, DirectX::XMMatrixTranspose(trans));
            DirectX::XMStoreFloat4x4(&new_impl.Rotation, DirectX::XMMatrixTranspose(rotate));
            DirectX::XMStoreFloat4x4(&new_impl.Scale, DirectX::XMMatrixTranspose(scale));

            pack.currFrameResource->GetResource<std::shared_ptr<Chen::CDX12::UploadBuffer<Transform::Impl>>>(
                "ObjTransformCB")->CopyData(p2obj.second->GetID()-1, new_impl);
            p2obj.second->GetProperty("Transform")->ClearOne();
        }
    }
}
