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

        void Init()
        {
            
        }

        void DebugOutputContents();

        std::filesystem::path& GetRootPath() { return assetRootPath; }
        std::filesystem::path& GetScenePath() { return defaultScenePath; }

    private:
        AssetMngr() = default;
        ~AssetMngr() = default;

        std::filesystem::path assetRootPath{ "../../assets" };
        std::filesystem::path defaultScenePath{"../../assets/json/DefaultScene.json"};
    };
}
