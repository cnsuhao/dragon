// RichEditBubble.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "RichEditBubble.h"

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

    void OnInitialize()
    {
        m_pRichEdit = (UI::IRichEdit*)this->FindChildObject(TEXT("richedit"));
        m_pRichEdit->SetReadOnly(true);

        // �μ�ࡣ���ڳſ���������֮��Ŀ�϶
        PARAFORMAT2 pf;
        m_pRichEdit->GetDefaultParaFormat(&pf);
        pf.dySpaceAfter = UI::Util::DXtoHimetricX(2);
        pf.dySpaceBefore = UI::Util::DXtoHimetricX(2);
        m_pRichEdit->SetDefaultParaFormat(&pf);

        LPCTSTR szMessage[] = 
        {
            TEXT("�����ֵ�ʱ����أ�"), TEXT("140."),
            TEXT("���ݵ�ʱ���أ�"), TEXT("�߽�����"),
            TEXT("�þò�����Ӱ�ˣ���֪�����ϵ��Ϻ�������û��"),
            TEXT("�����ˣ������߿��������������磬�����ص����������峤��������ѩ���������߸���«�ޡ�")
        };
        int nCount = sizeof(szMessage)/sizeof(LPCTSTR);
        for (int i = 0; i < nCount; i++)
        {
            if (0 == i%2)
            {
                HANDLE hHandle = m_pRichEdit->InsertMessageStart(UI::BUBBLE_NORMAL, UI::BUBBLE_ALIGN_LEFT);
                m_pRichEdit->AppendText(szMessage[i]);
                m_pRichEdit->InsertMessageFinish(hHandle);
            }
            else
            {
                HANDLE hHandle = m_pRichEdit->InsertMessageStart(UI::BUBBLE_NORMAL, UI::BUBBLE_ALIGN_RIGHT);
                m_pRichEdit->AppendText(szMessage[i]);
                m_pRichEdit->InsertMessageFinish(hHandle);
            }
        }
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

		String skinPath(szDir);
		skinPath.append(TEXT("Default"));
        g_pUIApplication->LoadSkinRes(skinPath.c_str());
    }

    DemoWindow* pWnd = NULL;
    DemoWindow::CreateInstance(g_pUIApplication->GetDefaultSkinRes(), &pWnd);
    if (pWnd->Create(TEXT("DemoWnd")))
    {
        pWnd->ShowWindow();
        g_pUIApplication->MsgHandleLoop();
    }
    SAFE_DELETE_Ixxx(pWnd);

    g_pUIApplication->Release();
    return 0;
}