#pragma once

#include <atomic>

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

    private:
        uint32_t instanceID;
        static std::atomic<uint32_t> CurrentID;
    };
}
