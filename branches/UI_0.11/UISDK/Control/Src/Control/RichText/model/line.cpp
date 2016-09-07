#include "stdafx.h"
#include "line.h"
#include "page.h"

using namespace UI;
using namespace UI::RichTextObjectModel;

// 新增一个行时，需要指定一个默认行高。这样在给一个空行添加一个回车时
// 不用再多判断一下给该行设置为默认行高。
Line::Line(Page* pPage, int nDefaultHeight)
{
    m_xInPage = 0;
    m_yInPage = 0;
    m_nHeight = nDefaultHeight;
	m_nRunsWidth = 0;
	m_bFirstParaLine = false;
	m_pPage = pPage;
	m_pFirstRun = NULL;
	m_pNextLine = NULL;
	m_pPrevLine = NULL;
}
Line::~Line()
{
	Run* p = m_pFirstRun;
	while (p)
	{
		Run* pSave = p;
		p = p->GetNextRun();

		delete pSave;
	}
	m_pFirstRun = NULL;
}

void  Line::GetRectInPage(RECT* prc)
{
	prc->left = m_xInPage;
	prc->top = m_yInPage;
	prc->right = prc->left + m_nRunsWidth;
	prc->bottom = prc->top + m_nHeight;
}

void  Line::GetRectInDoc(RECT* prc)
{
	GetRectInPage(prc);
	m_pPage->PageRect2DocRect(prc, prc);
}

void  Line::Draw(HDC hDC, RECT* prcPage)
{
    RECT rcLine;
    rcLine.left = prcPage->left + m_xInPage;
    rcLine.top = prcPage->top + m_yInPage;
    rcLine.right = prcPage->right;
    rcLine.bottom = rcLine.top + m_nHeight;

	Run* p = m_pFirstRun;
	while (p)
	{
		p->Draw(hDC, &rcLine);
		p = p->GetNextRun();
	}
}

Run*  Line::GetFirstRun()
{
	return m_pFirstRun;
}
Run*  Line::GetLastRun()
{
	Run* p = m_pFirstRun;
	while (p && p->GetNextRun())
		p = p->GetNextRun();

	return p;
}

bool  Line::HasRun()
{
	return m_pFirstRun?true:false;
}

Line*  Line::GetNextLine()
{
	return m_pNextLine;
}
Line*  Line::GetPrevLine()
{
	return m_pPrevLine;
}
void  Line::SetNextLine(Line* p)
{
	m_pNextLine = p;
}
void  Line::SetPrevLine(Line* p)
{
	m_pPrevLine = p;
}

Run*  Line::AppendRun(Element* pElement, uint nRunWidth, uint nRunHeight)
{
	if (!pElement)
		return NULL;

	Run* pRun = new Run(pElement, nRunWidth);
	if (m_pFirstRun)
	{
		Run* pLastRun = GetLastRun();
		pLastRun->SetNextRun(pRun);
		pRun->SetPrevRun(pLastRun);
	}
	else
	{
		m_pFirstRun = pRun;
	}
	
	pRun->OnAdd2Line(this);

	if (nRunHeight > m_nHeight)
		m_nHeight = nRunHeight;

	m_nRunsWidth += nRunWidth;
	return pRun;
}

void  Line::SetPosition(int xInPage, int yInPage)
{
    m_xInPage = xInPage;
    m_yInPage = yInPage;
}

void  Line::SetHeight(uint nHeight)
{
    m_nHeight = nHeight;
}

uint  Line::GetHeight()
{
    return m_nHeight;
}

void  Line::SetFirstParaLine(bool b)
{
	m_bFirstParaLine = b;
}

Run*  Line::RunHitTest(POINT ptInLine, __out POINT* ptInRun)
{
	int left = 0;
	int right = 0;

	Run* p = m_pFirstRun;
	while (p)
	{
		left = p->GetXInLine();
		right = left + p->GetWidth();
		if (ptInLine.x >= left && ptInLine.x <= right)
		{
			if (ptInRun)
			{
				ptInRun->x = ptInLine.x - left;
				ptInRun->y = ptInLine.y;
			}
			return p;
		}

		p = p->GetNextRun();
	}


	return NULL;
}