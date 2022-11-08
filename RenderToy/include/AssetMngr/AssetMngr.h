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

        void Init(std::filesystem::path rootPath)
        {
            assetRootPath = rootPath / "assets";
            shaderPath = rootPath / "shaders";
            defaultScenePath = rootPath / "assets/json/DefaultScene.json";
        }

        void DebugOutputContents();

        std::filesystem::path& GetRootPath() { return assetRootPath; }
        std::filesystem::path& GetShaderPath() { return shaderPath; }
        std::filesystem::path& GetScenePath() { return defaultScenePath; }

    private:
        AssetMngr() = default;

        ~AssetMngr() = default;

        std::filesystem::path assetRootPath;
        std::filesystem::path shaderPath;
        std::filesystem::path defaultScenePath;
    };
}
