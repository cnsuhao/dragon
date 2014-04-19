// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "UISDK\Project\UI3D\src\D3D\d3dapp.h"
#include "UISDK\Project\UI3D\src\D2D\d2dapp.h"
#include "UISDK\Project\UI3D\src\D3D\stage\stage3d.h"
#include "UISDK\Project\UI3D\src\D2D\d2drendertarget.h"
#include "UISDK\Project\UI3D\src\D2D\d2drenderbitmap.h"
#include "UISDK\Project\UI3D\src\D2D\d2drenderfont.h"

UI::IUIApplication*  g_pUIApplication = NULL;
HMODULE    g_hInstance = NULL;
D3D10App*  g_pD3DApp = NULL;
D2DApp*    g_pD2DApp = NULL;


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
		g_hInstance = hModule;
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

ID3D10Device*  GetDevice()
{
    if (g_pD3DApp)
        return g_pD3DApp->m_pDevice;

    return NULL;
}
namespace UI
{

extern "C"
{
    __declspec(dllexport) bool RegisterUIObject(IUIApplication* p)
    {
        REGISTER_UI_OBJECT(p, Stage3D);
        return true;
    }

    bool  UI3D_RegisterUIObject(IUIApplication* p)
    {
        return RegisterUIObject(p);
    }

	bool  UI3D_Init(IUIApplication* p, bool bInitD3D)
	{
		// d2d基于d3d，因此先初始化d3d
		if (bInitD3D && !g_pD3DApp)
		{
			g_pD3DApp = new D3D10App();
			g_pD3DApp->Init();
		}
		if (!g_pD2DApp)
		{
			g_pD2DApp = new D2DApp;
			g_pD2DApp->Init();
		}

		g_pUIApplication = p;
		return true;
	}

	bool  UI3D_Release()
	{
		if (g_pD2DApp)
		{
			g_pD2DApp->Release();
			SAFE_DELETE(g_pD2DApp);
		}
		if (g_pD3DApp)
		{
			g_pD3DApp->Release();
			SAFE_DELETE(g_pD3DApp);
		}
		return true;
	}

    // 创建D2D的渲染器指针
    bool  UI3D_CreateD2DRenderTarget(IRenderTarget**  ppOut)
    {
        if (!g_pD2DApp)
            return false;

        if (!ppOut)
            return false;

        *ppOut = new Direct2DRenderTarget;
        return true;
    }

    // 创建D2D图片指针
    bool  UI3D_CreateD2DRenderBitmap(IMAGE_ITEM_TYPE eType, IRenderBitmap**  ppOut)
    {
        if (!g_pD2DApp)
            return false;

        if (!ppOut)
            return false;

        switch (eType)
        {
        case IMAGE_ITEM_TYPE_ICON:
            *ppOut = new Direct2DIconRenderBitmap;
            break;

        case IMAGE_ITEM_TYPE_IMAGE_LIST:
            *ppOut = new Direct2DImageListRenderBitmap;
            break;

        case IMAGE_ITEM_TYPE_IMAGE:
        case IMAGE_ITEM_TYPE_GIF:
        case IMAGE_ITEM_TYPE_PNGLISTGIF:
            *ppOut = new Direct2DRenderBitmap;
            break;

        default:
            return false;
        }
        
        if (*ppOut)
            (*ppOut)->AddRef();

        return true;
    }

    bool  UI3D_CreateD2DRenderFont(IRenderFont**  ppOut)
    {
        if (!g_pD2DApp)
            return false;

        if (!ppOut)
            return false;

        *ppOut = new Direct2DRenderFont;
        (*ppOut)->AddRef();
        return true;
    }
};

}