// UIBuilder.cpp : ����Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Other\UIEditor.h"
#include "Dialog\Framework\MainFrame.h"

// ȫ�ֱ���:
CAppModule      _Module;
CGlobalData*    g_pGlobalData = NULL;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	
	g_pGlobalData = new CGlobalData;
	g_pGlobalData->Init();

	_Module.Init(NULL, hInstance);
	AtlAxWinInit();   // for webbrowser
	CMessageLoop  theLoop;
	_Module.AddMessageLoop(&theLoop);

	{
		// ����
		CMainFrame  frameWnd;
		frameWnd.Create( NULL, NULL, _T("UI Editor"), WS_OVERLAPPEDWINDOW );
		g_pGlobalData->m_hWndMainFrame = frameWnd.m_hWnd;
        frameWnd.HandleCmdLine(lpCmdLine);

		frameWnd.ShowWindow(SW_SHOWMAXIMIZED);
        //frameWnd.ShowWindow(SW_SHOW);

        g_pGlobalData->m_pMyUIApp->MsgHandleLoop();
// 		theLoop.Run();

	}

	SAFE_DELETE(g_pGlobalData);

	_Module.RemoveMessageLoop();
	_Module.Term();
	return (int) 0;
}

