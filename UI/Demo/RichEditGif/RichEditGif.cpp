// RichEditGif.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "RichEditGif.h"

#include <shlwapi.h>

#include "UISDK\Kernel\Inc\Base\inc.h"
#include "UISDK\Control\Inc\Base\inc.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Control\Inc\Interface\irichedit.h"
#include "UISDK\Project\UIUtil\h\Util.h"

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "uiutil.lib")
#pragma comment(lib, "uisdk.lib")
#pragma comment(lib, "uictrls.lib")
#pragma warning(disable:4996)

UI::IUIApplication*  g_pUIApplication = NULL;

class DemoWindow : public UI::IWindow
{
public:
    DemoWindow()
    {
        m_pRichEdit = NULL;
    }

    UI_BEGIN_MSG_MAP_Ixxx(DemoWindow)
        UIMSG_WM_DESTROY(OnDestroy)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(IWindow);

private:

    static void __stdcall __EnumFileInDirProc(LPCTSTR szDir, LPCTSTR szName, WPARAM wParam)
    {
        DemoWindow* pThis = (DemoWindow*)wParam;

        String strPath(szDir);
        strPath.append(szName);
        pThis->m_pRichEdit->InsertGif(strPath.c_str());
    }

    void OnInitialize()
    {
        m_pRichEdit = (UI::IRichEdit*)this->FindChildObject(TEXT("richedit"));

        TCHAR szPath[MAX_PATH] = {0};
        GetModuleFileName(GetModuleHandle(NULL), szPath, MAX_PATH);
        TCHAR* p = _tcsrchr(szPath, '\\');
        if(p != NULL)
            *(p+1) = 0;

        _tcscat(szPath, TEXT("emotion\\"));

        UI::Util::EnumFileInDirectory(
                szPath, 
                (EnumFileInDirProc)&DemoWindow::__EnumFileInDirProc,
                (WPARAM)this);
    }

    void OnDestroy()
    {
        PostQuitMessage(0); 
    }

private:
    UI::IRichEdit*  m_pRichEdit;
};

int APIENTRY _tWinMain(
                       HINSTANCE hInstance,
                       HINSTANCE hPrevInstance,
                       LPTSTR    lpCmdLine,
                       int       nCmdShow)
{

    TCHAR szDir[MAX_PATH] = {0};
    GetModuleFileName(GetModuleHandle(NULL), szDir, MAX_PATH);
    TCHAR* p = _tcsrchr(szDir, '\\');
    if(p != NULL)
        *(p+1) = 0;

    {
        TCHAR szUIDllPath[MAX_PATH] = {0};
        TCHAR szUIDllPath2[MAX_PATH] = {0};
        _tcscpy(szUIDllPath, szDir);
        _tcscat(szUIDllPath, TEXT("..\\..\\UI"));

        PathCanonicalize(szUIDllPath2, szUIDllPath);
        SetDllDirectory(szUIDllPath2);
    }

    TCHAR szSDKVersion[64] = {0};
    UI::UISDKVersion::GetVersionText(szSDKVersion, 64);
    CreateUIApplicationInstance(&g_pUIApplication);

    UICtrl_RegisterUIObject(g_pUIApplication);

    {
        TCHAR szLogXml[MAX_PATH];
        _tcscpy(szLogXml, szDir);
        _tcscat(szLogXml, TEXT("uilog.xml"));
        g_pUIApplication->LogUI(szLogXml);

        g_pUIApplication->SetSkinDirection(szDir);
        g_pUIApplication->LoadSkin(TEXT("Default"));
    }

    DemoWindow* pWnd = NULL;
    DemoWindow::CreateInstance(g_pUIApplication, &pWnd);
    if (pWnd->Create(TEXT("DemoWnd")))
    {
        pWnd->ShowWindow();
        g_pUIApplication->MsgHandleLoop();
    }
    SAFE_DELETE_Ixxx(pWnd);

    g_pUIApplication->Release();
    return 0;
}