#include <RenderToy/RenderToy.h>
#include <AssetMngr/AssetMngr.h>

using namespace Chen;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
    PSTR cmdLine, int showCmd)
{
    OutputDebugString(L"\n\nHello RenderToy\n\n");
    RToy::Asset::AssetMngr::GetInstance().DebugOutputContents(); 

//#if defined(DEBUG) | defined(_DEBUG)
//    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
//#endif
//
//    try
//    {
//        RToy::RenderToy Toy(hInstance);
//        if (!Toy.Initialize())
//            return 0;
//
//        return Toy.Run();
//    }
//    catch (DxException& e)
//    {
//        MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
//        return 0;
//    }

    return 0;
}