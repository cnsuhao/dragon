// SimpleDemo.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "SimpleDemo.h"

#include "UI/UISDK/Inc/inc.h"
#include "UI/UISDK/Inc/Util/util.h"
#pragma comment(lib, "UISDK.lib")

#include "UI/UICTRL/Inc/inc.h"
#pragma comment(lib, "UICTRL.lib")

#include "UI/UICTRL/Inc/Interface/ibutton.h"
#include "UI/UISDK/Inc/Interface/icustomwindow.h"
#include "UI/UICTRL/Inc/Interface/ilabel.h"
#include "UI/UICTRL/Inc/Interface/imenu.h"
#include "UI/UICTRL/Inc/Interface/iedit.h"
#include <atlcomcli.h>

class LoginWindow : public UI::IWindowMessageCallback
{
public:
	void  Create(UI::ISkinRes* pSkinRes)
	{
		m_pWindow = UI::ICustomWindow::CreateInstance(pSkinRes);
		m_pWindow->SetWindowMessageCallback(this);

		m_pWindow->Create(L"LoginWnd");
		m_pWindow->ShowWindow();
	}

protected:
	virtual BOOL  OnWindowMessage(UINT msg, WPARAM wParam, LPARAM, LRESULT& lResult)
	{
		if (msg == WM_DESTROY)
		{
			PostQuitMessage(0);
		}
		else if (msg == WM_MOUSEWHEEL)
		{
			int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			if (zDelta > 0)
				m_nCount = m_nCount + 1;
			else
				m_nCount = m_nCount - 1;

			std::wstring str = m_strName;
			str.append(L"-");
			m_strName.set(str.c_str());
		}
		return FALSE;
	}
	virtual BOOL  OnWindowUIMessage(UI::UIMSG* pMsg) override
	{
		if (pMsg->message == UI_MSG_INITIALIZE)
		{
			OnInitialize();
		}
		else if (pMsg->message == WM_CONTEXTMENU)
		{
			UI::LoadMenuData data = { 0 };
			data.pSkinRes = m_pWindow->GetSkinRes();
			data.hWndParent = m_pWindow->GetHWND();
			UI::IMenu* pMenu = UILoadMenu(&data);

			pMenu->AppendString(L"AAAA", 1);
			pMenu->AppendSeparator(0);
			pMenu->AppendString(L"BBBB", 2);

			POINT pt;
			GetCursorPos(&pt);
			pMenu->TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, 0);
			pMenu->Release();
		}
		return FALSE;
	}

	void  OnInitialize()
	{
		UI::ILabel* pLabel = (UI::ILabel*)m_pWindow->FindChildObject(TEXT("label1"));
		if (pLabel)
		{
			m_nCount.AddObserver(pLabel->GetListener(), UI::ILabel::PROPERTY_TEXT, 0, 0);
		}
		pLabel = (UI::ILabel*)m_pWindow->FindChildObject(TEXT("label2"));
		if (pLabel)
		{
			m_strName.AddObserver(pLabel->GetListener(), UI::ILabel::PROPERTY_TEXT, 0, 0);
		}

		UI::IButton* pBtn = (UI::IButton*)m_pWindow->FindChildObject(TEXT("btn"));
		if (pBtn)
		{
			pBtn->ClickedEvent().connect(this, &LoginWindow::on_click);
		}

		UI::IObject* pEdit = (UI::IObject*)m_pWindow->FindChildObject(TEXT("edit"));
		if (pEdit)
		{
			UI::PasswordEdit_SetRealPassword(pEdit, TEXT("123456"));
		}
	}
	void on_click(UI::IButton*)
	{
// 		UI::IObject* pEdit = (UI::IObject*)m_pWindow->FindChildObject(TEXT("edit"));
// 		if (pEdit)
// 		{
// 			ATL::CComBSTR bstr;
// 			UI::PasswordEdit_GetRealPassword(pEdit, &bstr);
// 			::OutputDebugString(bstr);
// 			::OutputDebugString(TEXT("\r\n"));
// 
// 			UI::PASSWORDEDIT_MODE eMode = UI::PASSWORDEDIT_MODE_PLAIN;
// 			UI::PasswordEdit_SwitchMode(pEdit, eMode);
// 			return;
// 		}

		static bool b = false;
		b = !b;

		if (b)
		{
			UI::LayerAnimateParam param = { 0 };
			param.bBlock = true;
			m_pWindow->GetLayer()->SetOpacity(50, &param);
		}
		else
		{
			UI::LayerAnimateParam param = { 0 };
			param.bBlock = false;
			m_pWindow->GetLayer()->SetOpacity(255, &param);
		}
	}

private:
	UI::ICustomWindow*  m_pWindow = nullptr;
	UI::bindable_int  m_nCount;
	UI::bindable_string  m_strName;
};

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UI::IUIApplication* pUIApp = NULL;
	UI::CreateUIApplication(&pUIApp);
	UICTRL_RegisterUIObject(pUIApp);

	// 顶级资源
	TCHAR szResPath[MAX_PATH] = { 0 };
	UI::Util::PathInBin(L"Skin\\Root", szResPath);
	UI::ISkinRes* pRootSkinRes = pUIApp->LoadSkinRes(szResPath);

	// 登录插件资源
	UI::Util::PathInBin(L"Skin\\Plugin\\Login", szResPath);
	UI::ISkinRes* pLoginSkinRes = pUIApp->LoadSkinRes(szResPath);

	// 绑定
	pLoginSkinRes->SetParentSkinRes(pRootSkinRes);

	LoginWindow m;
	m.Create(pLoginSkinRes);
	

// 	MainWindow m2;
// 	m2.Create(pSkinRes2);

	pUIApp->MsgHandleLoop();
	pUIApp->Release();
	
	return 0;
}