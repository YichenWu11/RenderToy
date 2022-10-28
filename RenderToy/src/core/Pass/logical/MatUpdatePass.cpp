#include <Pass/logical/MatUpdatePass.h>
#include <PropertyMngr/Material.h>

using namespace Chen::RToy;

MatUpdatePass::MatUpdatePass(std::string name) : IPass(name) 
{
    
}

MatUpdatePass::~MatUpdatePass()
{

}

void MatUpdatePass::Init(ID3D12Device* _device, ID3D12GraphicsCommandList* _cmdList)
{

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
                "MatIndexCB")->CopyData(p2obj.second->GetID(), new_id);
            p2obj.second->GetProperty("Material")->ClearOne();
        }
    }
}
