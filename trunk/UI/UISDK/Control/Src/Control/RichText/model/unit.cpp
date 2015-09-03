#include "stdafx.h"
#include "unit.h"
#include "element.h"
#include "doc.h"
#include "run.h"
#include "line.h"
#include "layout_context.h"

using namespace UI;
using namespace UI::RichTextObjectModel;

Unit::Unit(Doc* pDoc)
{
    m_pDoc = pDoc;
    m_bHover = false;
    m_bPress = false;
    m_pFirstElement = NULL;
	m_leftPadding = m_rightPadding = 0;
}
Unit::~Unit()
{
    Release_Element();
}

void  Unit::Release_Element()
{
    Element* p = m_pFirstElement;
    Element* pNext = NULL;

    while (p)
    {
        pNext = p->GetNextElement();
        delete p;
        p = pNext;
    }
    m_pFirstElement = NULL;
}

Doc*  Unit::GetDoc()
{
    return m_pDoc;
}

Element*  Unit::GetFirstElement()
{
    return m_pFirstElement;
}
Element*  Unit::GetLastElement()
{
    Element* p = m_pFirstElement;
    while (p && p->GetNextElement()) 
        p = p->GetNextElement();
    
    return p;
}

Element*  Unit::AppendElement()
{
    Element* pElement = this->CreateElement();
    if (!pElement)
        return NULL;

    AppendElement2(pElement);
    return pElement;
}

void  Unit::AppendElement2(Element* pElement)
{
    if (!m_pFirstElement)
    {
        m_pFirstElement = pElement;
    }
    else
    {
        Element* pLastElement = GetLastElement();
        pLastElement->SetNextElement(pElement);
        pElement->SetPrevElement(pLastElement);
    }
}

void  Unit::LayoutSingleLine(SingleLineLayoutContext* pContext)
{
	Element* pElement = m_pFirstElement;
	while (pElement)
	{
		int nRunWidth = pElement->GetWidth();
		int nRunHeight = pElement->GetHeight();

		Run* pRun = pContext->pLine->AppendRun(pElement, nRunWidth, nRunHeight);
		pRun->SetRange(0, pElement->GetMaxGlyphIndex());
		pRun->SetPosInLine(pContext->xLineOffset);

		pContext->xLineOffset += nRunWidth;
        pElement = pElement->GetNextElement();
	}
}

void  Unit::SetHover(bool b)
{
    m_bHover = b;
    Invalidate();
}
void  Unit::SetPress(bool b)
{
    m_bPress = b;
	Invalidate();
}
bool  Unit::IsHover()
{
    return m_bHover;
}
bool  Unit::IsPress()
{
    return m_bPress;
}

void  Unit::PreLayout()
{
	Element* pElement = m_pFirstElement;
	while (pElement)
	{
		pElement->ClearRun();
		pElement = pElement->GetNextElement();
	}
}

void  Unit::Invalidate()
{
    if (m_pDoc->GetPageMode() == SingleLine)
    {
        m_pDoc->InvalidRegion(NULL);
    }
    else
    {
        list<Line*>  listDirtyLine;

        Element* pElement = m_pFirstElement;
        while (pElement)
        {
			pElement->GetRelativeLines(listDirtyLine);
            pElement = pElement->GetNextElement();
        }

		// 整理成map，过滤重复
		map<Line*, bool>  mapDirtyLine;
		{
			list<Line*>::iterator iter = listDirtyLine.begin();
			for (; iter != listDirtyLine.end(); ++iter)
			{
				mapDirtyLine[(*iter)] = 1;
			}
		}

		// 刷新
		map<Line*, bool>::iterator iter = mapDirtyLine.begin();
		for (; iter != mapDirtyLine.end(); ++iter)
		{
			RECT rcPage;
			iter->first->GetRectInDoc(&rcPage);
			m_pDoc->InvalidRegion(&rcPage);
		}
    }
}