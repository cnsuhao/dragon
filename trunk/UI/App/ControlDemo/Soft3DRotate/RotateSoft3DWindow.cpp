#include "StdAfx.h"
#include "RotateSoft3DWindow.h"
#include "../Other/resource.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"
#include "UISDK\Control\Inc\Interface\ilistbox.h"
#include "UISDK\Control\Inc\Interface\istringlistitem.h"
#include "UISDK\Control\Inc\Interface\iiconstringlistitem.h"
#include "UISDK\Control\Inc\Interface\ilistview.h"
#include "UISDK\Control\Inc\Interface\iheaderctrl.h"
#include "UISDK\Control\Inc\Interface\ipropertyctrl.h"
#include "UISDK\Control\Inc\Interface\icombobox.h"
#include "UISDK\Kernel\Inc\Interface\iobject3dwrap.h"
#include "UISDK\Control\Inc\Interface\ilabel.h"

CSoft3DRotateWindow::CSoft3DRotateWindow()
{
	m_pCtrl = NULL;
	m_pCtrl3 = NULL;
	m_pBtnStart = NULL;
    m_pRotatePanel = NULL;
}

CSoft3DRotateWindow::~CSoft3DRotateWindow(void)
{
}

void CSoft3DRotateWindow::OnInitWindow()
{
    __super::nvProcessMessage(GetCurMsg(), 0, 0);

    HRESULT hr = SetCanDrop(true);
    UIASSERT(SUCCEEDED(hr));
    this->ModifyStyle(OBJECT_STYLE_RECEIVE_DRAGDROPEVENT, 0, false);

    m_pCtrl = (UI::IPictureCtrl*)FindChildObject(_T("pic"));
	m_pCtrl3 = (UI::IPictureCtrl*)FindChildObject(_T("pic3"));

    m_pBtnStart = (UI::IButton*)FindChildObject(_T("btnStart"));
    m_pRotatePanel = (UI::IButton*)FindChildObject(_T("RotatePanel"));
}

LRESULT  CSoft3DRotateWindow::OnDropTargetEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UI::DROPTARGETEVENT_DATA* pData = (UI::DROPTARGETEVENT_DATA*)lParam;
    UI::UI_LOG_DEBUG(_T("%s  type:%d"), FUNC_NAME, wParam);

    switch (wParam)
    {
    case UI::_DragEnter:
        break;

    case UI::_DragOver:
        break;

    case UI::_DragLeave:
        break;

    case UI::_Drop:
        break;
    }

    return 0;
}

void  CSoft3DRotateWindow::OnPaint(UI::IRenderTarget* pRenderTarget)
{
}

void  CSoft3DRotateWindow::Start()
{
	SetTimer(GetHWND(), 1, 30, NULL);

    if (m_pBtnStart)
    {
        m_pBtnStart->SetUserData((LPVOID)1);
        m_pBtnStart->SetText(_T("Pause"));
    }
}
void  CSoft3DRotateWindow::End()
{
	KillTimer(GetHWND(), 1);

    if (m_pBtnStart)
    {
        m_pBtnStart->SetUserData(0);
        m_pBtnStart->SetText(_T("Start"));
    }
}
void  CSoft3DRotateWindow::OnBnClicked(IMessage*  pMsgFrom)
{
    UI::IButton*  pBtn = static_cast<UI::IButton*>(pMsgFrom);
    const TCHAR* szId = pBtn->GetId();
    if (0 == _tcscmp(szId, _T("btnStart")))
    {
        long lData = (long)pBtn->GetUserData();
        if (0 == lData)
        {
    		Start();
        }
        else
        {
            End();
        }
        return;
    }
    else
    {
        MessageBox(GetHWND(), _T("click!"), _T(">.<"), MB_OK|MB_ICONINFORMATION);
    }
}
void  CSoft3DRotateWindow::OnTimer(UINT_PTR nIDEvent, LPARAM lParam)
{
    static int i = 0;
    i+=2;

    if (m_pCtrl)
	{
		m_pCtrl->GetRenderLayer2()->Rotate3D(0, i, 0);
        //m_pCtrl->UpdateObject();
        //m_pCtrl->GetRenderLayer2()->DirectCommit(true);
	}
    if (m_pRotatePanel)
    {
        m_pRotatePanel->GetRenderLayer2()->Rotate3D(i,0,i);
        //m_pRotatePanel->GetRenderLayer2()->DirectCommit(true);
    }
	if (m_pCtrl3)
	{
		m_pCtrl3->GetRenderLayer2()->Rotate3D(0, 0, i);
        //m_pCtrl3->UpdateObject();
        //m_pCtrl3->GetRenderLayer2()->DirectCommit(true);
	}
  
	this->UpdateObject();  // 注：这种场景下直接调用窗口的UpdateObject效率最高

	if (i % 360 == 0)
	{
		End();
	}
}