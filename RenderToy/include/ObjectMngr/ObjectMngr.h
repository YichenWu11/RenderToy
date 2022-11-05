#pragma once

#include <rapidjson/document.h> 
#include <rapidjson/filereadstream.h>

#include <CDX12/Math/MathHelper.h>
#include <CDX12/Math/Quaternion.h>

#include "./BasicObject.h"
#include <AssetMngr/AssetMngr.h>
#include <map>
#include <unordered_map>
#include <memory>
#include <set>

using namespace rapidjson;
using namespace Chen::CDX12;

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
            FILE* fp;
            errno_t err = fopen_s(&fp, Asset::AssetMngr::GetInstance().GetScenePath().string().c_str(), "rb");
            if (err == 0) OutputDebugString(L"\n\nFile Open Error!!!\n\n");

            char readBuffer[65536];
            FileReadStream is(fp, readBuffer, sizeof(readBuffer));

            Document document;
            document.ParseStream(is);

            assert(document.HasMember("objects"));

            const Value& objects = document["objects"];
            assert(objects.IsArray());

            for (SizeType idx = 0; idx < objects.Size(); ++idx)
            {
                std::string name = objects[idx]["name"].GetString();
                std::string matName = objects[idx]["matName"].GetString();
                std::string geoName = objects[idx]["geo"].GetString();
                std::string meshName = objects[idx]["mesh"].GetString();
                AddObject(std::make_shared<BasicObject>(name));
                // set material
                dynamic_cast<Material*>(GetObj(name)->GetProperty("Material"))->SetMaterial(
                    RenderResourceMngr::GetInstance().GetMatMngr()->GetMaterial(matName));
                // set mesh
                dynamic_cast<Mesh*>(GetObj(name)->GetProperty("Mesh"))->SetMeshGeo(geoName);
                dynamic_cast<Mesh*>(GetObj(name)->GetProperty("Mesh"))->SetSubMesh(meshName);
                // set layer
                dynamic_cast<BasicObject*>(GetObj(name))->SetLayer(ObjectLayer(objects[idx]["layer"].GetUint()));
                // set matTransform
                DirectX::XMFLOAT4X4 matTransform;
                DirectX::XMStoreFloat4x4(&matTransform, DirectX::XMMatrixScaling(
                    objects[idx]["mat_scale"]["x"].GetFloat(), 
                    objects[idx]["mat_scale"]["y"].GetFloat(), 
                    objects[idx]["mat_scale"]["z"].GetFloat()));
                dynamic_cast<Material*>(GetObj(name)->GetProperty("Material"))->SetMatTransform(matTransform);
                // set translate / scale / rotation
                DirectX::XMFLOAT4X4 scale;
                DirectX::XMFLOAT4X4 trans;
                DirectX::XMFLOAT4X4 rotation;
                DirectX::XMFLOAT3 axis;
                
                DirectX::XMStoreFloat4x4(&scale, DirectX::XMMatrixScaling(
                    objects[idx]["scale"]["x"].GetFloat(), 
                    objects[idx]["scale"]["y"].GetFloat(), 
                    objects[idx]["scale"]["z"].GetFloat()));
                DirectX::XMStoreFloat4x4(&trans, DirectX::XMMatrixTranslation(
                    objects[idx]["translate"]["x"].GetFloat(), 
                    objects[idx]["translate"]["y"].GetFloat(), 
                    objects[idx]["translate"]["z"].GetFloat()));

                axis.x = objects[idx]["rotation"]["axis"]["x"].GetFloat();
                axis.y = objects[idx]["rotation"]["axis"]["y"].GetFloat();
                axis.z = objects[idx]["rotation"]["axis"]["z"].GetFloat();
                DirectX::XMStoreFloat4x4(&rotation, DirectX::XMMatrixRotationQuaternion(
                    Math::Quaternion(
                        DirectX::XMLoadFloat3(&axis),
                        objects[idx]["rotation"]["angle"].GetFloat())));
                
                dynamic_cast<Transform*>(GetObj(name)->GetProperty("Transform"))->SetScale(scale);
                dynamic_cast<Transform*>(GetObj(name)->GetProperty("Transform"))->SetTranslate(trans);
                dynamic_cast<Transform*>(GetObj(name)->GetProperty("Transform"))->SetRotation(rotation);
            }

            fclose(fp);
        }   

        void AddObject(std::shared_ptr<IObject> p2obj)
        {
            if (mObjects.find(p2obj->GetID()) == mObjects.end())
            {
                mObjects[p2obj->GetID()] = p2obj;
                name2ID[p2obj->GetObjName()] = p2obj->GetID();
                nameList.emplace(p2obj->GetObjName());
                biggestID = p2obj->GetID();
            }
        }

        void DelObject(std::uint32_t id)
        {
            if (mObjects.find(id) != mObjects.end())
            {
                name2ID.erase(name2ID.find(mObjects.find(id)->second->GetObjName()));
                nameList.erase(nameList.find(mObjects.find(id)->second->GetObjName()));
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

        std::set<std::string>& GetNameList() { return nameList; }

    private:
        ObjectMngr() = default;
        ~ObjectMngr() = default;

        /*
            The initial of All objects.
            GUID ---> Object
        */
        std::map<std::uint32_t, std::shared_ptr<IObject>> mObjects; 
        std::unordered_map<std::string, std::uint32_t> name2ID;
        std::set<std::string> nameList;
        std::uint32_t biggestID {0};
    };
}
