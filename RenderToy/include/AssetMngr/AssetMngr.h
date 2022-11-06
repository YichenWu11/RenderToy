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
        std::filesystem::path& GetShaderPath() { return shaderPath; }
        std::filesystem::path& GetScenePath() { return defaultScenePath; }

    private:
        AssetMngr()
        {
            assetRootPath = std::filesystem::path("E:/GitProject/Mine/RenderToy/assets");
            shaderPath = std::filesystem::path("E:/GitProject/Mine/RenderToy/shaders");
            defaultScenePath = std::filesystem::path("E:/GitProject/Mine/RenderToy/assets/json/DefaultScene.json");
        }

        ~AssetMngr() = default;

        std::filesystem::path assetRootPath;
        std::filesystem::path shaderPath;
        std::filesystem::path defaultScenePath;
    };
}
