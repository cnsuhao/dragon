#include "stdafx.h"
#include "page.h"
#include "line.h"
#include "doc.h"

using namespace UI;
using namespace UI::RichTextObjectModel;

Page::Page(Doc* pDoc)
{
	m_pDoc = pDoc;
	m_pNextPage = NULL;
	m_pPrevPage = NULL;
	m_pFirstLine = NULL;

    m_posInDoc.x = m_posInDoc.y = 0;
    m_sizePage.cx = m_sizePage.cy = 0;
	m_bVisible = true;
}

Page::~Page()
{
	Line* p = m_pFirstLine;
	while (p)
	{
		Line* pSave = p;
		p = p->GetNextLine();

		delete pSave;
	}
	m_pFirstLine = NULL;
}

Page*  Page::GetNextPage()
{
	return m_pNextPage;
}
Page*  Page::GetPrevPage()
{
	return m_pPrevPage;
}

void  Page::Draw(HDC hDC, POINT ptDoc)
{
    RECT rcPage = {0};
    GetPageContentRegion(ptDoc, &rcPage);

	Line* pLine = m_pFirstLine;
	while (pLine)
	{
		pLine->Draw(hDC, &rcPage);
		pLine = pLine->GetNextLine();
	}
}

Line*  Page::GetFirstLine()
{
	return m_pFirstLine;
}

Line*  Page::GetLastLine()
{
	Line* p = m_pFirstLine;
	while (p && p->GetNextLine())
		p = p->GetNextLine();

	return p;
}

Line*  Page::AppendLine()
{
	Line* pLine = new Line(this, m_pDoc->GetDefaultCharFormat()->nFontHeight);
	if (m_pFirstLine)
	{
		Line* pLastLine = GetLastLine();
		pLastLine->SetNextLine(pLine);
		pLine->SetPrevLine(pLastLine);
	}
	else
	{
		m_pFirstLine = pLine;
	}

    return pLine;
}

void  Page::SetSize(int cx, int cy)
{
    m_sizePage.cx = cx;
    m_sizePage.cy = cy;
}

// 获取page内容区域(page可能有留白)
void  Page::GetPageContentRegion(POINT ptDoc, RECT* prc)
{
    if (!m_pDoc)
        return;

    const REGION4&  rPagePadding = m_pDoc->GetDefaultParaFormat()->rPagePadding;
    prc->left = ptDoc.x + m_posInDoc.x;
    prc->top = ptDoc.y + m_posInDoc.y;
    prc->right = prc->left + m_sizePage.cx;
    prc->bottom = prc->top + m_sizePage.cy;

    Util::DeflatRect(prc, &rPagePadding);
}

void  Page::SetVisible(bool b)
{
	m_bVisible = b;
}
bool  Page::IsVisible()
{
	return m_bVisible;
}

Line*  Page::LineHitTest(POINT ptInPage, __out POINT* ptInLine)
{
	RECT  rcLine;

	Line* pLine = m_pFirstLine;
	while (pLine)
	{
		pLine->GetRectInPage(&rcLine);
		if (PtInRect(&rcLine, ptInPage))
		{
			if (ptInLine)
			{
				ptInLine->x = ptInPage.x - rcLine.left;
				ptInLine->y = ptInPage.y - rcLine.top;
			}
			return pLine;
		}
		pLine = pLine->GetNextLine();
	}

	return NULL;
}

void  Page::PageRect2DocRect(RECT* prcPage, RECT* prcDoc)
{
	RECT rc;
	CopyRect(&rc, prcPage);
	OffsetRect(&rc, m_posInDoc.x, m_posInDoc.y);
	CopyRect(prcDoc, &rc);
}