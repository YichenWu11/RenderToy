#pragma once

#include "../IObject.h"
#include <map>
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

        void AddObject(std::shared_ptr<IObject> p2obj)
        {
            if (mObjects.find(p2obj->GetID()) == mObjects.end())
                mObjects[p2obj->GetID()] = p2obj;
        }

        void DelObject(std::uint32_t id)
        {
            if (mObjects.find(id) != mObjects.end())
                mObjects.erase(mObjects.find(id));
        }

    private:
        ObjectMngr() = default;
        ~ObjectMngr() = default;

        /*
            The initial of All objects.
            GUID ---> Object
        */
        std::map<std::uint32_t, std::shared_ptr<IObject>> mObjects; 
    };
}
