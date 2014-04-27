// ControlDemo.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "ControlDemo.h"
#include "../Soft3DRotate/RotateSoft3DWindow.h"
#include "../Clock/clockwindow.h"
#include "../REShadow/reshadow.h"
#include "../Direct2D/Direct2DWindow.h"
#include "../Stage3D_Leaves/StageLeavesWindow.h"
#include "../Animate/AnimateWindow.h" 
#include "../Animate2/AnimateWindow2.h" 
#include "../RenderLayer/RenderLayer.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"

#define MAX_LOADSTRING 100

// 全局变量:
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
HWND  g_hWnd = NULL;
UI::IUIApplication* g_pUIApp = NULL;
HINSTANCE g_hInstance = NULL;

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

class CGraphicsSelectDlg : public UI::IWindow
{
public:
	CGraphicsSelectDlg()
	{
		m_eType = UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
	}
	UI_BEGIN_MSG_MAP_Ixxx(CGraphicsSelectDlg)
		UIMSG_BN_CLICKED3(OnClick)
	UI_END_MSG_MAP_CHAIN_PARENT(UI::IWindow)

public:
	void  OnClick(IMessage*  pMsgFrom)
	{
		UI::IButton*  pBtn = static_cast<UI::IButton*>(pMsgFrom);

		if (0 == _tcscmp(XML_WINDOW_GRAPHICS_RENDER_LIBRARY_GDI, pBtn->GetId()))
			m_eType = UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
		else if (0 == _tcscmp(XML_WINDOW_GRAPHICS_RENDER_LIBRARY_GDIPLUS, pBtn->GetId()))
			m_eType = UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS;
		if (0 == _tcscmp(XML_WINDOW_GRAPHICS_RENDER_LIBRARY_D2D, pBtn->GetId()))
			m_eType = UI::GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D;

		EndDialog(IDOK);
	}

public:
	UI::GRAPHICS_RENDER_LIBRARY_TYPE  m_eType;
};

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
    g_hInstance = hInstance;

 	// TODO: 在此放置代码。
//	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CONTROLDEMO, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CONTROLDEMO));
	
	CreateUIApplicationInstance(&g_pUIApp);

	TCHAR szPath[MAX_PATH] = _T("");
    UI::Util::GetAppPath_(szPath);
	String str = szPath;

    g_pUIApp->SetSkinDirection(str.c_str());
	g_pUIApp->LoadSkin(_T("Default"));

    str.append(_T("\\Default\\skin.xml"));
    g_pUIApp->SetLog(NULL, (BSTR)str.c_str());

    UI_Ctrl_RegisterUIObject(g_pUIApp);

    {
#ifdef DEMO_CLOCK
        ClockWindow* pWnd = NULL;
        ClockWindow::CreateInstance(g_pUIApp, &pWnd);
    
        SetWindowPos(g_hWnd, 0, 50, 50, 630, 660, SWP_NOZORDER);
        pWnd->Attach(g_hWnd, _T("clockwin"));

#elif defined _RESHADOW
        REShadowWindow* pWnd = NULL;
        REShadowWindow::CreateInstance(g_pUIApp, &pWnd);
        SetWindowPos(g_hWnd, 0, 50, 50, 300, 300, SWP_NOZORDER);

        pWnd->Attach(g_hWnd, _T("reshadow"));

#elif defined DEMO_STAGE3D
        UI::UI3D_RegisterUIObject(g_pUIApp);

        CDirect2DWindow* pWnd = NULL;
        CDirect2DWindow::CreateInstance(g_pUIApp, &pWnd);
        SetWindowPos(g_hWnd, 0, 50, 50, 500, 500, SWP_NOZORDER);

        pWnd->Attach(g_hWnd, _T("stage3d"));

#elif defined DEMO_STAGELEAVES
		UI::UI3D_RegisterUIObject(g_pUIApp);

		CStageLeavesWindow* pWnd = NULL;
		CStageLeavesWindow::CreateInstance(g_pUIApp, &pWnd);
		SetWindowPos(g_hWnd, 0, 50, 0, 610, 860, SWP_NOZORDER);

		pWnd->Attach(g_hWnd, _T("stageparticle"));

#elif defined DEMO_ANIMATE
        CAnimateWindow* pWnd = NULL;
        CAnimateWindow::CreateInstance(g_pUIApp, &pWnd);

        pWnd->Attach(g_hWnd, _T("animate"));
        
#elif defined DEMO_ANIMATE2

		CGraphicsSelectDlg*  pDlg = NULL;
		CGraphicsSelectDlg::CreateInstance(g_pUIApp, &pDlg);
		if (IDCANCEL == pDlg->DoModal(_T("selectgraphicsdlg"), NULL, false))
		{
			SAFE_DELETE_Ixxx(pDlg);
			g_pUIApp->Release();
			return 0;
		}

		UI::GRAPHICS_RENDER_LIBRARY_TYPE eType = pDlg->m_eType;
		SAFE_DELETE_Ixxx(pDlg);

        CAnimateWindow2* pWnd = NULL;
        CAnimateWindow2::CreateInstance(g_pUIApp, &pWnd);
        SetWindowPos(g_hWnd, 0, 0, 0, 750, 550, SWP_NOZORDER|SWP_NOMOVE);

		pWnd->GetIWindowRender()->SetGraphicsRenderType(eType);
        pWnd->Attach(g_hWnd, _T("animate2"));

#elif defined DEMO_RENDERLAYER
        CRenderLayerWindow* pWnd = NULL;
        CRenderLayerWindow::CreateInstance(g_pUIApp, &pWnd);

		SetWindowPos(g_hWnd, 0, 0, 0, 700, 600, SWP_NOMOVE|SWP_NOZORDER);
        pWnd->Attach(g_hWnd, _T("renderlayer"));
#else

        CSoft3DRotateWindow* pWnd = NULL;
        CSoft3DRotateWindow::CreateInstance(g_pUIApp, &pWnd);
        
        //SetWindowPos(g_hWnd, 0, 50, 50, 300, 300, SWP_NOZORDER);
  		pWnd->Attach(g_hWnd, _T("mainwindow"));
        
#endif

        ShowWindow(g_hWnd, nCmdShow);
        UpdateWindow(g_hWnd);

        g_pUIApp->MsgHandleLoop();

        pWnd->Detach();
        pWnd->delete_this();
  	}
	g_pUIApp->Release();
	return  0;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//    仅当希望
//    此代码与添加到 Windows 95 中的“RegisterClassEx”
//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//    这样应用程序就可以获得关联的
//    “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CONTROLDEMO));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_CONTROLDEMO);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_CONTROLDEMO));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   g_hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 1200, 500, NULL, NULL, hInstance, NULL);

   if (!g_hWnd)
   {
      return FALSE;
   }

   // CreateWindow(_T("BUTTON"), _T("HostWnd Button"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|WS_TABSTOP, 500,500, 200,30, g_hWnd, (HMENU)100, NULL, NULL);
   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_PAINT:
		{
			PAINTSTRUCT ps = {0};
			hdc = BeginPaint(hWnd, &ps);
			// TODO: 在此添加任意绘图代码...
			EndPaint(hWnd, &ps);
		}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

