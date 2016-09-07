#include "stdafx.h"
#include "element.h"
#include "run.h"

using namespace UI;
using namespace UI::RichTextObjectModel;

Element::Element(Unit* pUnit)
{
    m_pUnit = pUnit;
	m_pPrevElement = NULL;
	m_pNextElement = NULL;
}

Element::~Element()
{

}

Unit*  Element::GetUnit()
{
    return m_pUnit;
}

void  Element::SetNextElement(Element* p)
{
	m_pNextElement = p;
}
void  Element::SetPrevElement(Element* p)
{
	m_pPrevElement = p;
}
Element*  Element::GetNextElement()
{
	return m_pNextElement;
}
Element*  Element::GetPrevElement()
{
	return m_pPrevElement;
}

void  Element::AddRun(Run* p)
{
	m_listRuns.push_back(p);
}
void  Element::ClearRun()
{
	m_listRuns.clear();
}

// 获取元素所在行
void  Element::GetRelativeLines(list<Line*>& listLines)
{
	list<Run*>::iterator iter = m_listRuns.begin();
	for (; iter != m_listRuns.end(); ++iter)
	{
		listLines.push_back((*iter)->GetRelativeLine());
	}
}

void  Element::DrawText(
		HDC hDC, RECT* prc, LPCTSTR szText, 
		ULONG nLen, const RichTextCharFormat* pcf)
{
	if (!pcf || !prc || !szText || !nLen)
		return;

	HFONT hfont = pcf->hFont;
	if (!hfont)
		return;

	COLORREF oldTextColor = 0, oldBackColor = 0;

	if (pcf->bTextColor)
	{
		oldTextColor = GetTextColor(hDC);
		SetTextColor(hDC, pcf->textColor);
	}
	if (pcf->bBackColor)
	{
		oldBackColor = GetBkColor(hDC);
		SetBkMode(hDC, OPAQUE);
		::SetBkColor(hDC, pcf->backColor);
	}
	else
	{
		SetBkMode(hDC, TRANSPARENT);
	}

	int x = prc->left;
	int y = prc->bottom - pcf->nFontHeight;  // 文字默认画在baseline上

	HFONT hOldFont = (HFONT)SelectObject(hDC, hfont);
	::TextOut(hDC, x, y, szText, nLen);
	SelectObject(hDC, hOldFont);

	if (pcf->bTextColor)
	{
		SetTextColor(hDC, oldTextColor);
	}
	if (pcf->bBackColor)
	{
		SetBkMode(hDC, TRANSPARENT);
		::SetBkColor(hDC, oldBackColor);
	}
}