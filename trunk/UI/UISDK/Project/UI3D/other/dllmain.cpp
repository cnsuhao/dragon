// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "UISDK\Project\UI3D\src\D3D\d3dapp.h"
#include "UISDK\Project\UI3D\src\D3D\stage3d.h"

D3DApp*  g_pD3DApp = NULL;
UI::IUIApplication*  g_pUIApplication = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

IDirect3DDevice9*  GetDevice()
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

	bool  UI3D_Init(IUIApplication* p)
	{
		if (g_pD3DApp)
			return false;

		g_pD3DApp = new D3DApp();
		g_pD3DApp->Init();

        g_pUIApplication = p;
		return true;
	}

	bool  UI3D_Release()
	{
		if (!g_pD3DApp)
			return false;

		g_pD3DApp->Clear();
		SAFE_DELETE(g_pD3DApp);
		return true;
	}
};

}