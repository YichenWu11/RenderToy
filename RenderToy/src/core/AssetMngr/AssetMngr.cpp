#include <AssetMngr/AssetMngr.h>
#include <CDX12/DXUtil.h>

using namespace Chen::RToy::Asset;
using namespace Chen::CDX12;
using namespace std::filesystem;

void AssetMngr::DebugOutputContents()
{
	if (!exists(assetRootPath))
	{
		OutputDebugString(L"not exist\n");
		return;
	}
	directory_entry entry(assetRootPath);	
	if (entry.status().type() == file_type::directory)	
		OutputDebugString(L"is_dir\n");
	directory_iterator list(assetRootPath);	        
	for (auto& it : list) {
		OutputDebugString(AnsiToWString(it.path().filename().string()).c_str());
		OutputDebugString(L"\n");
		OutputDebugString(AnsiToWString(it.path().relative_path().string()).c_str());
		OutputDebugString(L"\n");
	}
}
