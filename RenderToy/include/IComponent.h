#pragma once

#include <CDX12/DXUtil.h>
#include <CDX12/GCmdList.h>
#include <CDX12/FrameResource.h>
#include <set>
#include "IPass.h"

namespace Chen::RToy {
    class IComponent 
    {
    public:
        IComponent() = default;
        IComponent(const IComponent&) = delete;
        IComponent& operator=(const IComponent&) = delete;
        virtual ~IComponent() {}

        virtual void Init(ID3D12Device*) = 0;
        virtual void Tick() = 0;

        // TODO: AddObject and DelObject

        void AddPass(std::unique_ptr<IPass> ptr)
        {
            if (mPasses.find(ptr->GetName()) != mPasses.end()) return;
            NameList.emplace(ptr->GetName());
            mPasses[ptr->GetName()] = std::move(ptr);
        }

        void DelPass(std::string name)
        {
            if (mPasses.find(name) == mPasses.end()) return;
            mPasses.erase(mPasses.find(name));
            NameList.erase(NameList.find(name));
        }

        IPass* GetPass(std::string name)
        {
            return (mPasses.find(name) != mPasses.end()) ? mPasses.at(name).get() : nullptr;
        }

        void AddObjForAllPasses(IObject* p2obj)
        {
            for (auto& pass : mPasses)
            {
                pass.second->AddObject(p2obj);
            }
        }

        void DelObjForAllPasses(std::string name)
        {
            for (auto& pass : mPasses)
            {
                pass.second->DelObject(name);
            }
        }

        virtual void FillPack(std::any pack) {}

        virtual void FillPassPack() {}

        virtual void FillObjectsForPass() {}

        std::set<std::string>& GetPassNameList() { return NameList; }

    protected:
        ID3D12Device* device;
        bool isInit{ false };
        std::set<std::string> NameList;
        std::map<std::string, std::unique_ptr<IPass>> mPasses;
    };
}
