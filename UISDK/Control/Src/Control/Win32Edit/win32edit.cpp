#include "stdafx.h"
#ifdef COMPILE_Win32Edit
#include "Win32Edit.h"
#include "UISDK\Control\Inc\Interface\iedit.h"

using namespace UI;


Win32Edit::Win32Edit()
{
    m_pIWin32Edit = NULL;
    m_bShowPrompt = false;
    m_bShowEditWnd = false;
    m_hFontEditWindow = NULL;

    m_bNotifyStartEdit = false;
    m_bNotifyCancelEdit = false;
    m_bNotifyAcceptEdit = false;
    m_bNotifyDestroyEdit = false;

    m_lLimitText = 0;
    m_bByteLimit = false;
#ifdef _DEBUG
	m_lLimitText = 5;
	m_bByteLimit = true;
#endif
}

Win32Edit::~Win32Edit()
{
    SAFE_DELETE_GDIOBJECT(m_hFontEditWindow);
}


LRESULT  Win32Edit::OnInitialize(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    //m_hFontEditWindow = 
    return 0;
}

void  Win32Edit::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
	IRenderBase* pBkgndRender = m_pIWin32Edit->GetBkRender();
	if (!pBkgndRender)
		return;

	CRect rc(0, 0, m_pIWin32Edit->GetWidth(), m_pIWin32Edit->GetHeight());
	if (!m_pIWin32Edit->IsEnable())
	{
		pBkgndRender->DrawState(pRenderTarget, &rc, EDIT_BKGND_RENDER_STATE_DISABLE);
	}
	else if (m_pIWin32Edit->IsFocus() || m_pIWin32Edit->IsPress())
	{
		pBkgndRender->DrawState(pRenderTarget, &rc, EDIT_BKGND_RENDER_STATE_PRESS);
	}
	else if (m_pIWin32Edit->IsHover())
	{
		pBkgndRender->DrawState(pRenderTarget, &rc, EDIT_BKGND_RENDER_STATE_HOVER);
	}
	else
	{
		pBkgndRender->DrawState(pRenderTarget, &rc, EDIT_BKGND_RENDER_STATE_NORMAL);
	}
}
void  Win32Edit::OnPaint(IRenderTarget* pRenderTarget)
{
	bool  bNeedDrawText = false;
	UINT  nRenderState = 0;
	if (!m_pIWin32Edit->IsEnable())
	{
		bNeedDrawText = true;
		nRenderState = EDIT_BKGND_RENDER_STATE_DISABLE;
	}
	else if (m_pIWin32Edit->IsFocus() || m_pIWin32Edit->IsPress())
	{
	}
	else if (m_pIWin32Edit->IsHover())
	{
		bNeedDrawText = true;
		nRenderState = EDIT_BKGND_RENDER_STATE_HOVER;
	}
	else
	{
		bNeedDrawText = true;
		nRenderState = EDIT_BKGND_RENDER_STATE_NORMAL;
	}
	CRect rcClient;
	m_pIWin32Edit->GetObjectClientRect(&rcClient);

	// 文字
	IRenderFont* pFont = m_pIWin32Edit->GetRenderFont();
	if (GRAPHICS_RENDER_LIBRARY_TYPE_GDI == pFont->GetGraphicsRenderLibraryType())
	{
		if (m_pIWin32Edit->GetTextRender())
		{
			m_pIWin32Edit->GetTextRender()->DrawState(
				pRenderTarget, &rcClient, nRenderState, 
				m_strText.c_str());
		}
		return;
	}
	else
	{
		HFONT  hFont = pFont->GetHFONT();

		HDC    hDC = pRenderTarget->GetHDC();
		HFONT  hOldFont = (HFONT)SelectObject(hDC, hFont);

		COLORREF oldTextCol = ::SetTextColor(hDC, 0x00000000);

		CRect rcText = rcClient;
		rcText.top = rcClient.top + (rcClient.Height() - 16)/2;
		rcText.bottom = rcText.top + 16; // todo: 字符高度

		ExtTextOut( 
			hDC, rcClient.left, rcText.top, 
			ETO_CLIPPED, 
			&rcText, 
			m_strText.c_str(), 
			m_strText.length(), NULL);

		::SetTextColor(hDC, oldTextCol);
		::SelectObject(hDC, hOldFont);
	}

}

void  Win32Edit::SetNotifyStartEdit(bool b)
{
    m_bNotifyStartEdit = b;
}
void  Win32Edit::SetNotifyAcceptEdit(bool b)
{
    m_bNotifyAcceptEdit = b;
}
void  Win32Edit::SetNotifyCancelEdit(bool b)
{
    m_bNotifyCancelEdit = b;
}
void  Win32Edit::SetNotifyDestroyEdit(bool b)
{
    m_bNotifyDestroyEdit = b;
}

void  Win32Edit::DrawPrompt(HDC hDC, CRect rc)
{
    if (!m_bShowPrompt)
        return;

    if (m_strPrompt.empty())
        return;

//     Draw((OLE_HANDLE)hDC, rc, 
//         CT2W(m_strPrompt.c_str()), NULL);
}
void  Win32Edit::SetText(LPCTSTR szText, bool bUpdateNow)
{
    if (szText)
        m_strText = szText;
    else
        m_strText.clear();

    if (bUpdateNow)
    {
        m_pIWin32Edit->UpdateObject();
    }
}
void  Win32Edit::SetPromptText(LPCTSTR szText, bool bUpdateNow)
{
    if (szText)
        m_strPrompt = szText;
    else
        m_strPrompt.clear();

    if (bUpdateNow)
    {
        m_pIWin32Edit->UpdateObject();
    }
}


LRESULT  Win32Edit::OnStateChanged(UINT, WPARAM wParam, LPARAM)
{
    if (wParam & (OSB_HOVER | OSB_PRESS))
    {
        m_pIWin32Edit->UpdateObject();
    }

    return 0;
}

void  Win32Edit::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (!m_bShowEditWnd)
    {
        EditWrap* pEditWindow = StartEdit(false);

        MapWindowPoints(m_pIWin32Edit->GetHWND(), NULL, &point, 1);
        MapWindowPoints(NULL, pEditWindow->m_hWnd, &point, 1);
        PostMessage(pEditWindow->m_hWnd, WM_LBUTTONDOWN, nFlags, MAKELPARAM(point.x, point.y));

        m_pIWin32Edit->UpdateObject();
    }
}

bool  Win32Edit::IsEditing()
{
    return m_bShowEditWnd;
}

EditWrap*  Win32Edit::StartEdit(bool bSelAll)
{
    if (m_bShowEditWnd)
        return NULL;
    
    if (m_bNotifyStartEdit)
    {
        UIMSG  msg;
        msg.message = UI_WM_NOTIFY;
        msg.nCode = UI_WEN_STARTEDIT;
        msg.pMsgFrom = m_pIWin32Edit;
        msg.wParam = (WPARAM)m_pIWin32Edit;
        if (SER_CANCEL == m_pIWin32Edit->DoNotify(&msg))
            return NULL;
    }

    EditWrap* pEditWindow = EditWrap::NewInstance(this);;

    RECT rc;
    m_pIWin32Edit->GetClientRectInWindow(&rc);
    ::MapWindowPoints(m_pIWin32Edit->GetHWND(), NULL, 
        (LPPOINT)&rc, 2);

    pEditWindow->Create(&rc, m_pIWin32Edit->GetHWND());
    pEditWindow->SetFont(m_hFontEditWindow);
    pEditWindow->Show();  // 必须先显示，再settext，否则文字布局不正确
    pEditWindow->SetText(m_strText.c_str(), bSelAll);
    if (m_lLimitText > 0)
    {
        pEditWindow->SetLimitText(m_lLimitText, m_bByteLimit);
    }

//         MapWindowPoints(m_pIWin32Edit->GetHWND(), NULL, &point, 1);
//         MapWindowPoints(NULL, pEditWindow->m_hWnd, &point, 1);
//         PostMessage(pEditWindow->m_hWnd, WM_LBUTTONDOWN, nFlags, MAKELPARAM(point.x, point.y));

    m_bShowEditWnd = true;
    m_pIWin32Edit->UpdateObject();
	m_pEditWindow = pEditWindow;
	return pEditWindow;
}

// 立即停止编辑，并且不再发送通知。如被编辑的那一项ListItem正在被删除
void  Win32Edit::DiscardEditNow()
{
    if (!m_pEditWindow)
        return;

    m_pEditWindow->Destroy(EDF_DISCARD);
}

ACCEPT_EDIT_RESULT  Win32Edit::OnEditWindowPreDestroy(HWND hWnd, EditDestroyFlag eFlag)
{
    ACCEPT_EDIT_RESULT ret = AER_ACCEPT;

    if (eFlag == EDF_ACCEPT)
    {
        int nTextLength = GetWindowTextLength(hWnd) + 1;
        TCHAR*  pszText = new TCHAR[nTextLength];
        memset(pszText, 0, sizeof(TCHAR)*nTextLength);
        GetWindowText(hWnd, pszText, nTextLength);

        m_strText = pszText;
        SAFE_ARRAY_DELETE(pszText);

        if (m_bNotifyAcceptEdit)
        {
            UIMSG  msg;
            msg.message = UI_WM_NOTIFY;
            msg.nCode = UI_WEN_ACCEPTEDIT;
            msg.pMsgFrom = m_pIWin32Edit;
            msg.wParam = (WPARAM)m_pIWin32Edit;
            msg.lParam = (LPARAM)m_strText.c_str();
            ret = (ACCEPT_EDIT_RESULT)m_pIWin32Edit->DoNotify(&msg);
        }
    }
    else if (eFlag == EDF_CANCEL)
    {   
        if (m_bNotifyCancelEdit)
        {
            UIMSG  msg;
            msg.message = UI_WM_NOTIFY;
            msg.nCode = UI_WEN_CANCELEDIT;
            msg.pMsgFrom = m_pIWin32Edit;
            msg.wParam = (WPARAM)m_pIWin32Edit;
            m_pIWin32Edit->DoNotify(&msg);
        }
    }
    else if (eFlag == EDF_DISCARD)
    {
        if (m_bNotifyCancelEdit)
        {
            UIMSG  msg;
            msg.message = UI_WM_NOTIFY;
            msg.nCode = UI_WEN_CANCELEDIT;
            msg.pMsgFrom = m_pIWin32Edit;
            msg.wParam = (WPARAM)m_pIWin32Edit;
            m_pIWin32Edit->DoNotify(&msg);
        }
    }

    return ret;
}

void  Win32Edit::OnEditWindowDestroy()
{
    m_bShowEditWnd = false;
    m_pEditWindow = NULL;

    if (m_bNotifyDestroyEdit)
    {
        UIMSG  msg;
        msg.message = UI_WM_NOTIFY;
        msg.nCode = UI_WEN_EDITDESTROY;
        msg.pMsgFrom = m_pIWin32Edit;
        msg.wParam = (WPARAM)m_pIWin32Edit;
        m_pIWin32Edit->DoNotify(&msg);
    }
    m_pIWin32Edit->UpdateObject();    
}

#endif