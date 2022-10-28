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
            // dynamic_cast<Mesh*>(GetObj("box1")->GetProperty("Mesh"))->SetSubMesh("sphere");
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
