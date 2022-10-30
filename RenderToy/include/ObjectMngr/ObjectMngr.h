#pragma once

#include "./BasicObject.h"
#include <map>
#include <unordered_map>
#include <memory>

namespace Chen::RToy {
    class ObjectMngr
    {
    public:
        static ObjectMngr& GetInstance()
        {
            static ObjectMngr instance;
            return instance;
        }

        void Init()
        {
            // Add the default Objects.
            AddObject(std::make_shared<BasicObject>("box1"));
            DirectX::XMFLOAT4X4 scale_box;
            DirectX::XMStoreFloat4x4(&scale_box, DirectX::XMMatrixScaling(4.0, 4.0, 4.0));
            dynamic_cast<Transform*>(GetObj("box1")->GetProperty("Transform"))->SetScale(scale_box);
            dynamic_cast<Material*>(GetObj("box1")->GetProperty("Material"))->SetMaterial(
                Chen::CDX12::RenderResourceMngr::GetInstance().GetMatMngr()->GetMaterial("bricks"));

            AddObject(std::make_shared<BasicObject>("sphere1"));
            dynamic_cast<Mesh*>(GetObj("sphere1")->GetProperty("Mesh"))->SetSubMesh("sphere");
            DirectX::XMFLOAT4X4 world;
            DirectX::XMStoreFloat4x4(&world, DirectX::XMMatrixTranslation(10.0, 10.0, 10.0));
            DirectX::XMFLOAT4X4 scale;
            DirectX::XMStoreFloat4x4(&scale, DirectX::XMMatrixScaling(10.0, 10.0, 10.0));
            dynamic_cast<Transform*>(GetObj("sphere1")->GetProperty("Transform"))->SetTranslate(world);
            dynamic_cast<Transform*>(GetObj("sphere1")->GetProperty("Transform"))->SetScale(scale);
            dynamic_cast<Material*>(GetObj("sphere1")->GetProperty("Material"))->SetMaterial(
                Chen::CDX12::RenderResourceMngr::GetInstance().GetMatMngr()->GetMaterial("matForSphere"));

            AddObject(std::make_shared<BasicObject>("ground"));
            dynamic_cast<Mesh*>(GetObj("ground")->GetProperty("Mesh"))->SetSubMesh("grid");
            DirectX::XMFLOAT4X4 scale_ground;
            DirectX::XMStoreFloat4x4(&scale_ground, DirectX::XMMatrixScaling(5.0, 5.0, 5.0));
            DirectX::XMFLOAT4X4 world_ground;
            DirectX::XMStoreFloat4x4(&world_ground, DirectX::XMMatrixTranslation(0.0, -10.0, 0.0));
            DirectX::XMFLOAT4X4 mat_trans;
            DirectX::XMStoreFloat4x4(&mat_trans, DirectX::XMMatrixScaling(5.0, 5.0, 5.0));
            dynamic_cast<Transform*>(GetObj("ground")->GetProperty("Transform"))->SetScale(scale_ground);
            dynamic_cast<Transform*>(GetObj("ground")->GetProperty("Transform"))->SetTranslate(world_ground);
            dynamic_cast<Material*>(GetObj("ground")->GetProperty("Material"))->SetMaterial(
                Chen::CDX12::RenderResourceMngr::GetInstance().GetMatMngr()->GetMaterial("bricksForGround"));
            dynamic_cast<Material*>(GetObj("ground")->GetProperty("Material"))->SetMatTransform(mat_trans);
        }   

        void AddObject(std::shared_ptr<IObject> p2obj)
        {
            if (mObjects.find(p2obj->GetID()) == mObjects.end())
            {
                mObjects[p2obj->GetID()] = p2obj;
                name2ID[p2obj->GetObjName()] = p2obj->GetID();
                biggestID = p2obj->GetID();
            }
        }

        void DelObject(std::uint32_t id)
        {
            if (mObjects.find(id) != mObjects.end())
            {
                name2ID.erase(name2ID.find(mObjects.find(id)->second->GetObjName()));
                mObjects.erase(mObjects.find(id));
            }
        }

        size_t GetObjNum() { return mObjects.size(); }

        std::uint32_t GetBiggestID() { return biggestID; }

        IObject* GetObj(std::uint32_t id) 
        {
            return (mObjects.find(id) != mObjects.end()) ? mObjects.at(id).get() : nullptr;
        }

        IObject* GetObj(std::string name)
        {
            return (name2ID.find(name) != name2ID.end()) ? GetObj(name2ID[name]) : nullptr;
        }

    private:
        ObjectMngr() = default;
        ~ObjectMngr() = default;

        /*
            The initial of All objects.
            GUID ---> Object
        */
        std::map<std::uint32_t, std::shared_ptr<IObject>> mObjects; 
        std::unordered_map<std::string, std::uint32_t> name2ID;
        std::uint32_t biggestID {0};
    };
}
