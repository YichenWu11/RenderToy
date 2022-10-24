#pragma once

#include "../IProperty.h"
#include <CDX12/RenderResourceMngr.h>
#include <CDX12/Math/MathHelper.h>
#include <CDX12/Geometry/MeshMngr.h>

using namespace Chen::CDX12::Math;
using namespace Chen::CDX12;

namespace Chen::RToy {
    class Mesh : public IProperty
    {
    public:
        Mesh(std::string meshName = std::string("box"), std::string name = std::string("Mesh")) : IProperty(name) 
        {
            // default : box
            SetMeshGeo(RenderResourceMngr::GetInstance().GetMeshMngr()->GetMeshGeo(meshName));
        }
        ~Mesh() = default;

        void SetMeshGeo(MeshGeometry* p) { impl.pMesh = p; }
        void SetMeshGeo(std::string meshName) 
        { 
            SetMeshGeo(RenderResourceMngr::GetInstance().GetMeshMngr()->GetMeshGeo(meshName));
        }
        void SetIndexCount(UINT count)   { impl.IndexCount = count; }
        void SetStartIndexLocation(UINT start) { impl.StartIndexLocation = start; }
        void SetBaseVertexLocation(UINT base)  { impl.BaseVertexLocation = base; }
        void SetTopology(D3D12_PRIMITIVE_TOPOLOGY t) { impl.PrimitiveType = t; }
        
        MeshGeometry* GetMeshGeo() { return impl.pMesh; }
        std::string GetMeshName()  { return impl.pMesh->Name; }
        UINT GetIndexCount() { return impl.IndexCount; }
        UINT GetStartIndexLocation() { return impl.StartIndexLocation; }
        UINT GetBaseVertexLocation() { return impl.BaseVertexLocation; }
        D3D12_PRIMITIVE_TOPOLOGY GetTopology() { return impl.PrimitiveType; }

        std::any GetImpl() override { return impl; }

        void Tick(std::any params) override 
        {
            // Do nothing
        }

        struct Impl
        {
            MeshGeometry* pMesh;
            // DrawIndexedInstanced parameters.
            UINT IndexCount = 0;
            UINT StartIndexLocation = 0;
            int BaseVertexLocation = 0;
	        D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; // Primitive topology.
        };
    private:
        Impl impl;
    };
}
