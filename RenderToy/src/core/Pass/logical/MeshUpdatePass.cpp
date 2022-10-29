/*
    ^ Do nothing.
    ^ Not allow to change the mesh of object
*/

#include <Pass/logical/MeshUpdatePass.h>
#include <PropertyMngr/Mesh.h>
#include <Utility/Macro.h>

using namespace Chen::RToy;

MeshUpdatePass::MeshUpdatePass(std::string name) : IPass(name) 
{
    AddObject(GetObjectMngr().GetObj("box1"));
    AddObject(GetObjectMngr().GetObj("sphere1"));
}

MeshUpdatePass::~MeshUpdatePass()
{

}

void MeshUpdatePass::Init(ID3D12Device* _device)
{
    device = _device;
}

void MeshUpdatePass::Tick()
{
    /*
        * The Mesh of the object only needs to be set at drawing time, 
        * so there is no need to upload it to the GPU
    */

    for (auto &p2obj : mObjects)
    {
        if (p2obj.second->GetProperty("Mesh")->IsDirty())
        {
            // Do something
            p2obj.second->GetProperty("Mesh")->ClearOne();
        }
    }
}
