#pragma once

#include <filesystem>

namespace Chen::RToy::Asset {
    class AssetMngr
    {
    public:
        static AssetMngr& GetInstance()
        {
            static AssetMngr instance;
            return instance;
        }

        void DebugOutputContents();

    private:
        AssetMngr() = default;
        ~AssetMngr() = default;

        std::filesystem::path assetRootPath{"../../assets"};
    };
}
