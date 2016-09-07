#include "stdafx.h"
#include "link_element.h"
#include "..\doc.h"

using namespace UI;
using namespace UI::RichTextObjectModel;

void  LinkElement::Draw(HDC hDC, uint nStart, uint nEnd, RECT* prcRun)
{
	UIASSERT(nStart <= nEnd && nEnd < m_strText.length());

	const RichTextCharFormat* pcf = this->GetCharFormat();
	LPCTSTR szText = m_strText.c_str() + nStart;
	ULONG nLen = nEnd - nStart + 1;

	__super::DrawText(hDC, prcRun, szText, nLen, pcf);

    const Style& style = m_pUnit->GetDoc()->GetStyle();

    // »®Ïß
    if (style.draw_link_underline)
    {
        if (m_pUnit->IsPress() ||/*&&*/ m_pUnit->IsHover())
        {
            HFONT hfont = pcf->hFont;
            if (!hfont)
                return;

            COLORREF color = RGB(0,0,0);
            if (pcf->bTextColor)
                color = pcf->textColor;

            int y = prcRun->bottom - pcf->nFontDescent + 1;

            HPEN hPen = CreatePen(PS_SOLID, 1, color);
            HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
            MoveToEx(hDC, prcRun->left, y, NULL);
            LineTo(hDC, prcRun->right, y);
            SelectObject(hDC, hOldPen);
            DeleteObject(hPen);
        }
    }
	
	return;
}

//////////////////////////////////////////////////////////////////////////

LinkUnit::LinkUnit(Doc* p)
	:TextUnit(p)
	,m_oIRichTextLinkUnit(this)
{
	m_wParam = m_lParam = 0;
}

void  LinkUnit::SetId(LPCTSTR szId)
{
    SETSTRING(m_strId, szId)
}
LPCTSTR  LinkUnit::GetId()
{
    return m_strId.c_str();
}

Element*  LinkUnit::CreateElement()
{
    return new LinkElement(this);
}

BOOL  LinkUnit::OnSetCursor()
{
	static HCURSOR sHandCursor = ::LoadCursor(NULL, IDC_HAND);
	SetCursor(sHandCursor);
	return TRUE;
}

void  LinkUnit::OnClick()
{
	UI::IMessage* pMsgObj = m_pDoc->GetIMessageObj();
	if (!pMsgObj)
		return;

	UIMSG  msg;
	msg.pMsgFrom = pMsgObj;
	msg.message = UI_WM_NOTIFY;
	msg.nCode = UI_RICHTEXT_NM_LICK_CLICK;
	msg.wParam = (WPARAM)GetIRichTextLinkUnit();
	pMsgObj->DoNotify(&msg);
}

LPCTSTR  LinkUnit::GetToolTipText()
{
	return m_strToolTip.c_str(); 
}

void  LinkUnit::SetToolTip(LPCTSTR szToolTip)
{
	SETSTRING(m_strToolTip, szToolTip)
}
void  LinkUnit::SetWParam(WPARAM w)
{
	m_wParam = w;
}
void  LinkUnit::SetLParam(LPARAM l)
{
	m_lParam = l;
}
WPARAM  LinkUnit::GetWParam()
{
	return m_wParam;
}
LPARAM  LinkUnit::GetLParam()
{
	return m_lParam;
}