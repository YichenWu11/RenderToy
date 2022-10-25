#pragma once

#include <CDX12/DXUtil.h>
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

        virtual void Init(ID3D12Device*, ID3D12GraphicsCommandList*) = 0;
        virtual void Tick() = 0;

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

        virtual void FillPack(std::any pack) {}

        virtual void FillPassPack() {}

        std::set<std::string>& GetPassNameList() { return NameList; }

    protected:
        ID3D12Device* device;
        ID3D12GraphicsCommandList* cmdList;
        bool isInit{ false };
        std::set<std::string> NameList;
        std::map<std::string, std::unique_ptr<IPass>> mPasses;
    };
}
