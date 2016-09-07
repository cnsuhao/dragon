// ListCtrlAnimate.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ListCtrlAnimate.h"

#include <shlwapi.h>

#include "UISDK\Kernel\Inc\Base\inc.h"
#include "UISDK\Control\Inc\Base\inc.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Project\UIUtil\h\Util.h"
#include "UISDK\Control\Inc\Interface\ilistview.h"

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "UxTheme.lib")
#pragma comment(lib, "uiutil.lib")
#pragma comment(lib, "uisdk.lib")
#pragma comment(lib, "uictrls.lib")
#pragma warning(disable:4996)

UI::IUIApplication*  g_pUIApplication = NULL;
UI::ISkinRes*  g_pSkinRes = NULL;
TCHAR  g_szAppDir[MAX_PATH] = {0};

class DemoWindow : public UI::IWindow
{
public:
    DemoWindow()
    {
    }

    UI_BEGIN_MSG_MAP_Ixxx(DemoWindow)
		UIMSG_BN_CLICKED(TEXT("btn"), OnClick)
        UIMSG_WM_DESTROY(OnDestroy)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(IWindow);

private:

    void OnInitialize()
    {
		SetWindowTheme(GetHWND(), L"explorer", NULL);

        UI::IListView*  pListView = (UI::IListView*)
				this->FindChildObject(TEXT("listview_test"));
        if (pListView)
        {
            UI::IHeaderListCtrl* pHeader = pListView->GetHeaderCtrl();
            if (pHeader)
            {
                pHeader->AddColumn(TEXT("First"), 80);
                pHeader->AddColumn(TEXT("Second"), 80);
                pHeader->AddColumn(TEXT("Third"), 80);

                for (int i = 0; i < 100; i++)
                {
                    UI::IListViewItemBase* pItem =
							pListView->AddTextItem(TEXT("Anna"), 0);
                    pListView->SetItemText(i, 1, TEXT("Nokia"), false);
                    pListView->SetItemText(i, 2, TEXT("C601"), false);
                }

                pListView->LayoutItem(0, false);
            }
        }
    }

	void OnClick()
	{
		String skinPath(g_szAppDir);
#if 0
		skinPath.append(TEXT("Plugin"));
#else
		skinPath.append(TEXT("Plugin.skn"));
#endif
		UI::ISkinRes* pPluginSkinRes = g_pUIApplication->LoadSkinRes(skinPath.c_str());

		if (pPluginSkinRes)
		{
			DemoWindow* pWnd = NULL;
			DemoWindow::CreateInstance(pPluginSkinRes, &pWnd);
			pWnd->DoModal(TEXT("DemoWnd"),GetHWND(), false);
			SAFE_DELETE_Ixxx(pWnd);
		}
	}

    void OnDestroy()
    {
		LPCTSTR szSkinResName = GetSkinRes()->GetName();
		if (szSkinResName && 0 == _tcscmp(szSkinResName, TEXT("Default")))
		{
			PostQuitMessage(0); 
		}
    }

private:
};

int APIENTRY _tWinMain(
                       HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{
	GetModuleFileName(GetModuleHandle(NULL), g_szAppDir, MAX_PATH);
	_tcsrchr(g_szAppDir, '\\')[1] = 0;

    {
        TCHAR szUIDllPath[MAX_PATH] = {0};
        TCHAR szUIDllPath2[MAX_PATH] = {0};
        _tcscpy(szUIDllPath, g_szAppDir);
        _tcscat(szUIDllPath, TEXT("..\\..\\UI"));

        PathCanonicalize(szUIDllPath2, szUIDllPath);
        SetDllDirectory(szUIDllPath2);
    }

    CreateUIApplicationInstance(&g_pUIApplication);
    UICtrl_RegisterUIObject(g_pUIApplication);

    {
        TCHAR szLogXml[MAX_PATH];
        _tcscpy(szLogXml, g_szAppDir);
        _tcscat(szLogXml, TEXT("uilog.xml"));
        //g_pUIApplication->LogUI(szLogXml);

		String skinPath(g_szAppDir);
		skinPath.append(TEXT("Default"));
        g_pSkinRes = g_pUIApplication->LoadSkinRes(skinPath.c_str());
    }

    DemoWindow* pWnd = NULL;
    DemoWindow::CreateInstance(g_pSkinRes, &pWnd);
    if (pWnd->Create(TEXT("DemoWnd")))
    {
        pWnd->ShowWindow();
        g_pUIApplication->MsgHandleLoop();
    }
    SAFE_DELETE_Ixxx(pWnd);

    g_pUIApplication->Release();
    return 0;
}