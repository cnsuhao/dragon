// ImageRotate3D.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "ImageRotate3D.h"
#include <shlwapi.h>

#include "UISDK\Kernel\Inc\Base\inc.h"
#include "UISDK\Control\Inc\Base\inc.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Control\Inc\Interface\ilabel.h"
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "uisdk.lib")
#pragma comment(lib, "uictrls.lib")
#pragma warning(disable:4996)

UI::IUIApplication*  g_pUIApplication = NULL;

class DemoWindow : public UI::IWindow
{
public:
    DemoWindow()
    {
        m_pPic = NULL;
    }

    UI_BEGIN_MSG_MAP_Ixxx(DemoWindow)
		UIMSG_WM_ANIMATE_TICK(OnAnimateTick)
        UIMSG_WM_DESTROY(OnDestroy)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(IWindow);

private:
    void OnInitialize()
    {
        m_pPic = (UI::IPictureCtrl*)__super::FindChildObject(TEXT("pic"));
		if (!m_pPic)
			return;

        __super::SetHardComposite(true);

        m_pPic->SetRenderLayer(true);
        UI::IRenderLayer*  pRenderLayer = m_pPic->GetRenderLayer2();
        if (pRenderLayer)
        {
            pRenderLayer->GetTransform3D(true)->perspective(2000);
        }

		UI::IAnimateManager*  pAnimate = g_pUIApplication->GetAnimateMgr();
		UI::IStoryboard* pStoryboard = pAnimate->CreateStoryboard(this);
		pStoryboard->CreateNoneTimeline(0);
		pStoryboard->Begin();
    }

    void OnDestroy()
    {
		g_pUIApplication->GetAnimateMgr()->ClearStoryboardOfNotify(this);
        PostQuitMessage(0); 
    }

	void  OnAnimateTick(int nCount, UI::IStoryboard** ppArray)
	{
		static float lAngle = 0;
		lAngle++;
		if (lAngle > 360)
			lAngle = 0;

		UI::IRenderLayer*  pRenderLayer = m_pPic->GetRenderLayer2();
		pRenderLayer->GetTransform3D(true)->rotateY(lAngle);

		if (this->IsHardComposite())
			this->DirectComposite();
		else
			this->UpdateObject();
	}

private:
    UI::IPictureCtrl*  m_pPic;
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

#if 1  // 启用硬件合成 
    g_pUIApplication->EnableGpuComposite();
#endif

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