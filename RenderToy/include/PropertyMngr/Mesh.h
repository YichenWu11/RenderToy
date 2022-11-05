#pragma once

#include <IProperty.h>
#include <CDX12/RenderResourceMngr.h>
#include <CDX12/Geometry/MeshMngr.h>

using namespace Chen::CDX12::Math;
using namespace Chen::CDX12;

namespace Chen::RToy {
    class Mesh : public IProperty
    {
    public:
        Mesh(std::string geoName = "shapeGeo", std::string meshName = "box", std::string name = "Mesh") : IProperty(name) 
        {
            // default : box
            SetMeshGeo(RenderResourceMngr::GetInstance().GetMeshMngr()->GetMeshGeo(geoName));
            SetSubMesh(meshName);
        }
        ~Mesh() = default;

        void SetMeshGeo(MeshGeometry* p) { impl.pMesh = p; SetDirty(); }
        void SetMeshGeo(std::string meshName) 
        { 
            SetMeshGeo(RenderResourceMngr::GetInstance().GetMeshMngr()->GetMeshGeo(meshName));
            SetDirty();
        }
        void SetIndexCount(UINT count)   { impl.IndexCount = count; SetDirty(); }
        void SetStartIndexLocation(UINT start) { impl.StartIndexLocation = start; SetDirty(); }
        void SetBaseVertexLocation(UINT base)  { impl.BaseVertexLocation = base; SetDirty(); }
        void SetTopology(D3D12_PRIMITIVE_TOPOLOGY t) { impl.PrimitiveType = t; SetDirty(); }
        void SetBoundingBox(DirectX::BoundingBox bounds) { impl.Bounds = bounds; SetDirty(); }
        void SetSubName(std::string n) { impl.subName = n; }

        void SetSubMesh(std::string meshName)
        {
            if (impl.pMesh->DrawArgs.find(meshName) == impl.pMesh->DrawArgs.end()) return;
            SetIndexCount(impl.pMesh->DrawArgs[meshName].IndexCount);
            SetStartIndexLocation(impl.pMesh->DrawArgs[meshName].StartIndexLocation);
            SetBaseVertexLocation(impl.pMesh->DrawArgs[meshName].BaseVertexLocation);
            // Topology : D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST [Default]
            SetBoundingBox(impl.pMesh->DrawArgs[meshName].Bounds);  
            SetSubName(meshName);
        }
        
        MeshGeometry* GetMeshGeo() { return impl.pMesh; }
        std::string GetMeshName()  { return impl.pMesh->Name; }
        UINT GetIndexCount() { return impl.IndexCount; }
        UINT GetStartIndexLocation() { return impl.StartIndexLocation; }
        UINT GetBaseVertexLocation() { return impl.BaseVertexLocation; }
        D3D12_PRIMITIVE_TOPOLOGY GetTopology() { return impl.PrimitiveType; }
        DirectX::BoundingBox GetBoundingBox() { return impl.Bounds; }
        std::string GetSubName() { return impl.subName; }

        std::any GetImpl() override { return impl; }

        struct Impl
        {
            MeshGeometry* pMesh;
            // DrawIndexedInstanced parameters.
            UINT IndexCount = 0;
            UINT StartIndexLocation = 0;
            int BaseVertexLocation = 0;
	        D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; // Primitive topology.
            DirectX::BoundingBox Bounds;
            std::string subName;
        };
    private:
        Impl impl;
    };
}
