#include "stdafx.h"
#include "richtext.h"
#include "parser\htmlparser.h"

using namespace UI;

RichText::RichText()
{
	m_pIRichText = NULL;
    m_bNeedLayout = false;
	m_pMKMgr = NULL;
}

RichText::~RichText()
{

}

void  RichText::SetIRichText(IRichText*  p)
{
	m_pIRichText = p;
	SetIMessageProxy(static_cast<IMessage*>(p));
}

void  RichText::OnInitialize()
{
	m_doc.Init(m_pIRichText);
	m_pMKMgr = m_doc.GetMKMgr();
}

void  RichText::OnPaint(IRenderTarget* pRT)
{
    if (m_bNeedLayout)
    {
        CRect rc;
        m_pIRichText->GetClientRectInObject(&rc);
        int  w = rc.Width();
        int  h = rc.Height();

        if (w < 0)
            w = 0;
        if (h < 0)
            h = 0;
        m_doc.LayoutAll(w, h);

        SetLayoutDirty(false);
    }

	HDC hDC = pRT->GetHDC();
	POINT pt = {0, 0};
	m_doc.Render2DC(hDC, pt);
}

void  RichText::OnSize(UINT nType, int cx, int cy)
{
    SetMsgHandled(FALSE);
    SetLayoutDirty(true);
}

void  RichText::OnMouseMove(UINT nFlags, POINT point)
{
	POINT ptDoc = {0};
	m_pIRichText->WindowPoint2ObjectClientPoint(&point, &ptDoc, true);

	if (m_pMKMgr)
		m_pMKMgr->OnMouseMove(nFlags, ptDoc);
	return ;
}

void  RichText::OnMouseLeave()
{
	if (m_pMKMgr)
		m_pMKMgr->OnMouseLeave();
	return ;
}

BOOL  RichText::OnSetCursor(HWND hWnd, UINT nHitTest, UINT message)
{
	if (m_pMKMgr)
	{
		if (!m_pMKMgr->OnSetCursor())
			SetMsgHandled(FALSE);
	}
	else
	{
		SetMsgHandled(FALSE);
	}
	return TRUE;
}

void  RichText::OnLButtonDown(UINT nFlags, POINT point)
{
	if (m_pMKMgr)
	{
		m_pMKMgr->OnLButtonDown(nFlags, point);
	}
}

void  RichText::OnLButtonUp(UINT nFlags, POINT point)
{
	if (m_pMKMgr)
	{
		m_pMKMgr->OnLButtonUp(nFlags, point);
	}
}

void  RichText::LoadHtml(LPCTSTR szHtml)
{
	if (!szHtml)
		return;

	m_doc.Release();

	HtmlParser parse(m_doc);
	parse.Parse(szHtml);
}

void  RichText::AppendText(LPCTSTR szText)
{
	m_doc.AppendText(szText);
    SetLayoutDirty(true);
}

void  RichText::AppendText2(LPCTSTR szText, RichTextCharFormat* pcf)
{
    m_doc.AppendText2(szText, pcf);
    SetLayoutDirty(true);
}

void  RichText::AppendImage(LPCTSTR szPath)
{
	m_doc.AppendImage(szPath);
	SetLayoutDirty(true);
}

void  RichText::AppendLink(RichTextLinkInfo* pInfo)
{
	m_doc.AppendLink(pInfo);
	SetLayoutDirty(true);
}

void  RichText::AppendLink2(RichTextLinkInfo* pInfo, RichTextCharFormat* pcf)
{
	m_doc.AppendLink2(pInfo, pcf);
	SetLayoutDirty(true);
}

void  RichText::GetDefaultCharFormat(RichTextCharFormat* pcf)
{
    if (!pcf)
        return;

    m_doc.CopyDefaultCharFormat(pcf);
}

void  RichText::GetDefaultLinkCharFormat(RichTextCharFormat* pcf)
{
	if (!pcf)
		return;

	m_doc.CopyDefaultCharFormat(pcf);
}

void  RichText::SetLayoutDirty(bool b)
{
    m_bNeedLayout = b;
}