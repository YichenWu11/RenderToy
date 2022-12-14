#pragma once

#include "IObject.h"
#include <CDX12/RenderResourceMngr.h>
#include <CDX12/FrameResource.h>
#include <CDX12/GCmdList.h>
#include <CDX12/Resource/UploadBuffer.h>
#include <CDX12/Math/MathHelper.h>
#include <set>
#include <map>

namespace Chen::RToy {
    class IPass
    {
    public:
        IPass(std::string name) : passName(name) {}
        IPass(const IPass&) = delete;
        IPass& operator=(const IPass&) = delete;
        virtual ~IPass() {}    

        virtual void Init(ID3D12Device*) = 0;
        virtual void Tick() = 0;

        std::string GetName() { return passName; }

        void AddObject(IObject* p2obj)
        {       
            if (mObjects.find(p2obj->GetObjName()) != mObjects.end()) return;
            mObjects[p2obj->GetObjName()] = p2obj;
            NameList.emplace(p2obj->GetObjName());
        }

        void DelObject(std::string name)
        {
            if (mObjects.find(name) == mObjects.end()) return;
            mObjects.erase(mObjects.find(name));
            NameList.erase(NameList.find(name));
        }

        virtual void FillPack(std::any pack) {}

        std::set<std::string>& GetObjNameList() { return NameList; }

        // *********************************************************************
        // TODO: Basic Method to Draw Obj

    protected:
        ID3D12Device* device;
        std::string passName;
        std::set<std::string> NameList;
        std::map<std::string, IObject*> mObjects;
    };
}
