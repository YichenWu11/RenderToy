#pragma once

#include "IProperty.h"
#include <atomic>
#include <map>
#include <any>
#include <cassert>

/*
    The Top Base class of all objects.
    Every Object owns a GUID.
*/

namespace Chen::RToy {
    class IObject
    {
    public:
        IObject() { instanceID = ++CurrentID; }
        IObject(const IObject&) = delete;
        IObject& operator=(const IObject&) = delete;
        IObject(IObject&&) = default;
        IObject& operator=(IObject&&) = default;
        virtual ~IObject() {}

        // call the Tick of every its property Every Frame If necessary

        void AddProperty(IProperty* property)
        {
            if (mProperties.find(property->GetName()) != mProperties.end()) return;
                mProperties[property->GetName()] = property;
        }

        void DelProperty(std::string name)
        {
            if (mProperties.find(name) != mProperties.end()) mProperties.erase(mProperties.find(name));
        }

        IProperty* GetProperty(std::string name)
        {
            return (mProperties.find(name) != mProperties.end()) ? mProperties.at(name) : nullptr;
        }

        template<typename T>
        typename T::Impl GetPropertyImpl(std::string name)
        {
            assert(mProperties.find(name) != mProperties.end());
            auto impl = mProperties.find(name)->second->GetImpl();
            return std::any_cast<typename T::Impl>(impl);
        }

        uint32_t GetID() { return instanceID; }

    private:
        uint32_t instanceID;
        static std::atomic<uint32_t> CurrentID;
        std::map<std::string, IProperty*> mProperties;
    };
}
