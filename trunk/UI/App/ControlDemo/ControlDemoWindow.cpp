#include "StdAfx.h"
#include "ControlDemoWindow.h"
#include "resource.h"
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

CControlDemoWindow::CControlDemoWindow()
{
	m_pCtrl = NULL;
	m_pCtrl2 = NULL;
	m_pCtrl3 = NULL;
	m_pBtnStart = NULL;
	m_pBtnEnd = NULL;
}

CControlDemoWindow::~CControlDemoWindow(void)
{
}

void CControlDemoWindow::OnInitWindow()
{
    __super::nvProcessMessage(GetCurMsg(), 0, 0);

    HRESULT hr = SetCanDrop(true);
    UIASSERT(SUCCEEDED(hr));
    this->ModifyStyle(OBJECT_STYLE_RECEIVE_DRAGDROPEVENT, 0, false);

    m_pCtrl = (UI::IPictureCtrl*)FindChildObject(_T("pic"));
	m_pCtrl2 = (UI::IPictureCtrl*)FindChildObject(_T("pic2"));
	m_pCtrl3 = (UI::IPictureCtrl*)FindChildObject(_T("pic3"));

    m_pBtnStart = (UI::IButton*)FindChildObject(_T("btnStart"));
    m_pBtnEnd = (UI::IButton*)FindChildObject(_T("btnEnd"));
    if (m_pBtnEnd)
    {
        m_pBtnEnd->SetEnable(false, false);
    }
    
}

LRESULT  CControlDemoWindow::OnDropTargetEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
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

void  CControlDemoWindow::OnPaint(UI::IRenderTarget* pRenderTarget)
{
}

void  CControlDemoWindow::Start()
{
	SetTimer(GetHWND(), 1, 100, NULL);
	if (m_pCtrl)
		m_pCtrl->Begin3D();
	if (m_pCtrl2)
		m_pCtrl2->Begin3D();
	if (m_pCtrl3)
		m_pCtrl3->Begin3D();

// 	         if (m_pCtrl)
// 	             m_pCtrl->Get3DWrap()->Rotate(0, 0, 45);
// 	         if (m_pCtrl2)
// 	             m_pCtrl2->Get3DWrap()->Rotate(0, 0, 45);
// 	         if (m_pCtrl3)
// 	             m_pCtrl3->Get3DWrap()->Rotate(0, 0, 45);
// 	         this->UpdateObject();

	if (m_pBtnEnd)
		m_pBtnEnd->SetEnable(true, true);
	if (m_pBtnStart)
		m_pBtnStart->SetEnable(false, true);

}
void  CControlDemoWindow::End()
{
	KillTimer(GetHWND(),1);
	if (m_pCtrl)
		m_pCtrl->End3D();
	if (m_pCtrl2)
		m_pCtrl2->End3D();
	if (m_pCtrl3)
		m_pCtrl3->End3D();

	if (m_pBtnEnd)
		m_pBtnEnd->SetEnable(false, false);
	if (m_pBtnStart)
		m_pBtnStart->SetEnable(true, false);

	this->UpdateObject(true);
}
void  CControlDemoWindow::OnBnClicked(IMessage*  pMsgFrom)
{
    const TCHAR* szId = static_cast<UI::IButton*>(pMsgFrom)->GetId();
    if (0 == _tcscmp(szId, _T("btnStart")))
    {
		Start();
        return;
    }
    else if (0 == _tcscmp(szId, _T("btnEnd")))
    {
        End();
        return;
    }
}
void  CControlDemoWindow::OnTimer(UINT_PTR nIDEvent, LPARAM lParam)
{
    static int i = 0;
    i += 2;

    if (m_pCtrl)
        m_pCtrl->Get3DWrap()->Rotate(0, i, 0);
	if (m_pCtrl2)
		m_pCtrl2->Get3DWrap()->Rotate(i, 0, 0);
	if (m_pCtrl3)
		m_pCtrl3->Get3DWrap()->Rotate(0, 0, i);

	this->UpdateObject();

	if (i > 720)
	{
		End();
	}
}