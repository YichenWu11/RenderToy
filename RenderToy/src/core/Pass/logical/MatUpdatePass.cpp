#include <Pass/logical/MatUpdatePass.h>
#include <PropertyMngr/Material.h>
#include <Utility/Macro.h>

using namespace Chen::RToy;

MatUpdatePass::MatUpdatePass(std::string name) : IPass(name) 
{
    AddObject(GetObjectMngr().GetObj("box1"));
    AddObject(GetObjectMngr().GetObj("sphere1"));
    AddObject(GetObjectMngr().GetObj("ground"));
}

MatUpdatePass::~MatUpdatePass()
{

}

void MatUpdatePass::Init(ID3D12Device* _device)
{
    device = _device;
}

void MatUpdatePass::Tick()
{
    for (auto& p2obj : mObjects)
    {
        // Tick Every Frame
        auto matImpl = p2obj.second->GetPropertyImpl<Material>("Material");
        Material::ID new_id;
        new_id.matIndex = matImpl.material->MatIndex;
        pack.currFrameResource->GetResource<std::shared_ptr<Chen::CDX12::UploadBuffer<Material::ID>>>(
            "MatIndexCB")->CopyData(p2obj.second->GetID() - 1, new_id);
    }
}
