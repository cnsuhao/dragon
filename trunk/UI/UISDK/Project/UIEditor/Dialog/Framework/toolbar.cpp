#include "stdafx.h"
#include "toolbar.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"
#include "Dialog\Framework\mainframe.h"
#include "Business\Command\Base\commandhistorymgr.h"

CToolBar::CToolBar()
{
    m_pBtnSave = NULL;
    m_pBtnNewWindow = NULL;
    m_pBtnUndo = NULL;
    m_pBtnRedo = NULL;
    m_pBtnPreview = NULL;
}

CToolBar::~CToolBar()
{

}

BOOL  CToolBar::PreCreateWindow(CREATESTRUCT* pcs)
{
    pcs->style = WS_CHILD;
    pcs->dwExStyle = 0;

    return TRUE;
}

void  CToolBar::OnInitialize()
{
    IWindow::nvProcessMessage(GetCurMsg(), 0, false);

    m_pBtnSave = (IButton*)FindChildObject(_T("toolbar_btn_save"));
    m_pBtnNewWindow = (IButton*)FindChildObject(_T("toolbar_btn_newwindow"));
    m_pBtnUndo = (IButton*)FindChildObject(_T("toolbar_btn_undo"));
    m_pBtnRedo = (IButton*)FindChildObject(_T("toolbar_btn_redo"));
    m_pBtnPreview = (IButton*)FindChildObject(_T("toolbar_btn_preview"));
}

void  CToolBar::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
    CRect  rc;
    this->GetClientRectInObject(&rc);

    UI::Color cBack(241,237,237,255);
    UI::Color cTopLine(145,145,145,255);
    UI::Color cBorder(0xFFFFFFFF);

    rc.bottom--;
    pRenderTarget->DrawRect(&rc, &cBack);

    //rc.bottom++;
    IRenderPen* pPen = pRenderTarget->CreateSolidPen(1, &cTopLine);
    pRenderTarget->DrawLine(rc.left, rc.bottom, rc.right, rc.bottom, pPen);
    SAFE_RELEASE(pPen);
}

long*  CToolBar::GetCurViewKey()
{
    return g_pGlobalData->m_pMainFrame->GetCurViewKey();
}

void  CToolBar::OnDirtyChanged(bool bDirty)
{
    if (!m_pBtnSave)
        return;

    m_pBtnSave->SetEnable(bDirty);
}
void  CToolBar::OnProjectOpened()
{
    if (m_pBtnNewWindow)
        m_pBtnNewWindow->SetEnable(true);
}

void  CToolBar::OnUndoRedoCountChanged(long* pKey)
{
	if (pKey == GetCurViewKey())
	{
		UpdateBtnState();
	}
}

void  CToolBar::UpdateBtnState()
{
	int  nUndo = 0;
	int  nRedo = 0;
	if (GetCurViewKey())
	{
        m_pBtnPreview->SetEnable(true, false);

        g_pGlobalData->m_pCmdHistroyMgr->GetUndoRedoCount(GetCurViewKey(), &nUndo, &nRedo);
	}
    else
    {
        m_pBtnPreview->SetEnable(false, false);
    }

	if (m_pBtnUndo)
		m_pBtnUndo->SetEnable(nUndo > 0?true:false, false);
	if (m_pBtnRedo)
		m_pBtnRedo->SetEnable(nRedo > 0?true:false, false);

    this->UpdateObject();
}

void  CToolBar::OnProjectClosed()
{
    if (m_pBtnNewWindow)
        m_pBtnNewWindow->SetEnable(false);
}

void  CToolBar::OnBtnSave()
{
    ::SendMessage(g_pGlobalData->m_pMainFrame->m_hWnd, WM_COMMAND,
        IDM_SAVE, (LPARAM)GetHWND());
}
void  CToolBar::OnBtnNewWindow()
{
    ::SendMessage(g_pGlobalData->m_pMainFrame->m_hWnd, WM_COMMAND,
        ID_NEW_WINDOW, (LPARAM)GetHWND());
}
void  CToolBar::OnBtnUndo()
{
    ::SendMessage(g_pGlobalData->m_pMainFrame->m_hWnd, WM_COMMAND,
        IDM_UNDO, (LPARAM)GetHWND());
}

void  CToolBar::OnBtnRedo()
{
    ::SendMessage(g_pGlobalData->m_pMainFrame->m_hWnd, WM_COMMAND,
        IDM_REDO, (LPARAM)GetHWND());
}

void  CToolBar::OnBtnPreview()
{
    g_pGlobalData->m_pMainFrame->ForwardCommnd2CurrentView(MDIVIEW_MESSAGE_PREVIEW, 0, 0);
}

// 使用快捷键触发一个命令时，需要检查当前是否可以执行该命令
bool  CToolBar::TestCommandIsValid(long ld)
{   
    switch (ld)
    {
    case IDM_SAVE:
        {
            if (!m_pBtnSave || !m_pBtnSave->IsEnable())
                return false;
        }
        break;

    case IDM_REDO:
        {
            if (!m_pBtnRedo || !m_pBtnRedo->IsEnable())
                return false;
        }
        break;

    case IDM_UNDO:
        {
            if (!m_pBtnUndo || !m_pBtnUndo->IsEnable())
                return false;
        }
        break;
    }

    return true;
}
