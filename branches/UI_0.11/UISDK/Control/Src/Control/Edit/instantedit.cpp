#include "stdafx.h"
#include "instantedit.h"

using namespace UI;

InstantEdit::InstantEdit()
{
    m_pIInstantEdit = NULL;
    m_bEditing = false;
}
InstantEdit::~InstantEdit()
{
    
}

LRESULT  InstantEdit::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetMsgHandled(FALSE);
	if (!IsRealFocus())
	{
		if (beginEdit())
			m_pIInstantEdit->UpdateObject();
	}
	return 0;
}

LRESULT  InstantEdit::OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetMsgHandled(FALSE);
	if (!IsRealFocus())
	{
		if (beginEdit())
			m_pIInstantEdit->UpdateObject();
	}
    return 0;
}
LRESULT  InstantEdit::OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (m_bEditing)
    {
        SetMsgHandled(FALSE);
        return 0;
    }

    SetCursor(LoadCursor(NULL, IDC_ARROW));
    return 0;
}

void  InstantEdit::OnSetFocus(HWND wndFocus)
{
	// ���ǻ����ʵ��
	;
}

void  InstantEdit::OnKillFocus(HWND wndFocus)
{
	// ���ǻ����ʵ��
	if (m_bEditing)
	{
		acceptEdit();
		m_pIInstantEdit->UpdateObject();
	}
}

void  InstantEdit::OnPaint(IRenderTarget* pRenderTarget)
{
    if (m_bEditing)
    {
        SetMsgHandled(FALSE);
        return;
    }

    if (!m_pIEdit->GetTextRender())
        return;

    // edit�ڲ��ǵ���ExtTextOut�����ı��ģ�ֻ�ܲ���top align����
    CRect rcDraw;
    m_pIEdit->GetObjectClientRect(&rcDraw);

    int nTextHeight = __super::GetTextHeight();
    rcDraw.top = rcDraw.top + (rcDraw.Height()-nTextHeight)/2;    
    rcDraw.bottom = rcDraw.top + nTextHeight;

    LPCTSTR szText = GetText();
    m_pIEdit->GetTextRender()->DrawState(pRenderTarget, &rcDraw, 
        EDIT_TEXTREDNER_STATE_NORMAL, szText, 
        DT_LEFT|DT_END_ELLIPSIS|DT_NOPREFIX|DT_SINGLELINE);
}

void  InstantEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_ESCAPE)
	{
		if (m_bEditing)
		{
			cancelEdit();
			m_pIInstantEdit->UpdateObject();
			return;
		}
	}
	else if (nChar == VK_RETURN)
	{
		if (m_bEditing)
		{
			acceptEdit();
			m_pIInstantEdit->UpdateObject();
			return;
		}
	}

	SetMsgHandled(FALSE);
	return;
}

bool  InstantEdit::beginEdit()
{
	UIMSG msg;
	msg.message = UI_WM_NOTIFY;
	msg.nCode = UI_EN_STARTEDIT;
	msg.pMsgFrom = m_pIInstantEdit;

	START_EDIT_RESULT  eResult = (START_EDIT_RESULT)m_pIInstantEdit->DoNotify(&msg);
	if (eResult == START_EDIT_CANCEL)
		return false;

	IMessageFilterMgr* pMgr = m_pIEdit->GetUIApplication()->GetMessageFilterMgr();
	if (pMgr)
		pMgr->AddMessageFilter(static_cast<IPreTranslateMessage*>(this));

	m_strTextBeforeEdit = GetText();
	::SetCursor(::LoadCursor(NULL, IDC_IBEAM));

	EnterFocus();
	m_bEditing = true;
	return true;
}
void  InstantEdit::cancelEdit()
{
	SetText(m_strTextBeforeEdit.c_str(), false);

	UIMSG msg;
	msg.message = UI_WM_NOTIFY;
	msg.nCode = UI_EN_CANCELEDIT;
	msg.pMsgFrom = m_pIInstantEdit;
	m_pIInstantEdit->DoNotify(&msg);

	endEdit();
}
void  InstantEdit::acceptEdit()
{
	// ����û�иı�ʱ��ֱ�ӽ�������֪ͨ�ⲿ
	if (m_strTextBeforeEdit == GetText())
	{
		endEdit();
		return;
	}

	UIMSG msg;
	msg.message = UI_WM_NOTIFY;
	msg.nCode = UI_EN_ACCEPTEDIT;
	msg.pMsgFrom = m_pIInstantEdit;

	ACCEPT_EDIT_RESULT  eResult = (ACCEPT_EDIT_RESULT)m_pIInstantEdit->DoNotify(&msg);
	if (eResult == ACCEPT_EDIT_REJECT)
	{
		SetText(m_strTextBeforeEdit.c_str(), false);
		endEdit();
	}
	else if (eResult == ACCEPT_EDIT_REJECT_AND_CONTINUE_EDIT)
	{
		m_pIInstantEdit->SetFocusInWindow();
	}
	else
	{
		endEdit();
	}
}	
void  InstantEdit::endEdit()
{
	IMessageFilterMgr* pMgr = m_pIEdit->GetUIApplication()->GetMessageFilterMgr();
	if (pMgr)
		pMgr->RemoveMessageFilter(static_cast<IPreTranslateMessage*>(this));

	m_strTextBeforeEdit.clear();

	UIMSG msg;
	msg.message = UI_WM_NOTIFY;
	msg.nCode = UI_EN_ENDEDIT;
	msg.pMsgFrom = m_pIInstantEdit;
	m_pIInstantEdit->DoNotify(&msg);
    
    // ����Ƶ��ı����룬�����´ε��ʱ��
    // ����ʾ����һ�ε�ѡ��״̬��λ�ã������˸
    SetSel(0, 0);

	m_bEditing = false;
	ExitFocus();
	return;
}


BOOL  InstantEdit::PreTranslateMessage(MSG* pMsg)
{
	switch (pMsg->message)
	{
		// �������λ�ã�ֹͣ�༭
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
		{
			POINT  ptWnd = {GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam)};

			if (m_bEditing)
			{
				RECT rcWnd;
				m_pIEdit->GetWindowRect(&rcWnd);
				if (!PtInRect(&rcWnd, ptWnd))
				{
					acceptEdit();
					m_pIInstantEdit->UpdateObject();
				}
			}
		}
		break;

	case WM_KILLFOCUS:
		{
			if (pMsg->hwnd == m_pIEdit->GetHWND())
			{
				if (m_bEditing)
				{
					acceptEdit();
					m_pIInstantEdit->UpdateObject();
				}
			}
		}
		break;
	}
	return FALSE;
}