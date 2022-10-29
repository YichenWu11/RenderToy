#include <Pass/logical/MatUpdatePass.h>
#include <PropertyMngr/Material.h>
#include <Utility/Macro.h>

using namespace Chen::RToy;

MatUpdatePass::MatUpdatePass(std::string name) : IPass(name) 
{
    AddObject(GetObjectMngr().GetObj("box1"));
    AddObject(GetObjectMngr().GetObj("sphere1"));
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
    for (auto &p2obj : mObjects)
    {
        if (p2obj.second->GetProperty("Material")->IsDirty())
        {
            auto matImpl = p2obj.second->GetPropertyImpl<Material>("Material");
            Material::ID new_id(matImpl.material->MatIndex);
            // TODO: create a CB(register for every frameResource)： MatIndexCB，store the material index of obj
            pack.currFrameResource->GetResource<std::shared_ptr<Chen::CDX12::UploadBuffer<Material::ID>>>(
                "MatIndexCB")->CopyData(p2obj.second->GetID()-1, new_id);
            p2obj.second->GetProperty("Material")->ClearOne();
        }
    }
}
