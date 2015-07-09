#include "stdafx.h"
#include "run.h"
#include "element.h"

using namespace UI;
using namespace UI::RichTextObjectModel;

Run::Run(Element* p, int w)
{
	m_pLine = NULL;
	m_pElement = p;
	m_nStartIndexInElement = 0;
	m_nEndIndexInElement = 0;
	m_pNextRun = m_pPrevRun = NULL;
    m_xInLine = 0;
    m_w = w;
}

Run::~Run()
{

}

Run*  Run::GetNextRun()
{	
	return m_pNextRun;
}
Run*  Run::GetPrevRun()
{
	return m_pPrevRun;
}
void  Run::SetNextRun(Run* p)
{
	m_pNextRun = p;
}
void  Run::SetPrevRun(Run* p)
{
	m_pPrevRun = p;
}

void  Run::OnAdd2Line(Line* p)
{
	m_pLine = p;
	m_pElement->AddRun(this);
}

Line*  Run::GetRelativeLine()
{
	return m_pLine;
}

void  Run::SetRange(uint nStart, uint nEnd)
{
    UIASSERT(nEnd >= nStart);

	m_nStartIndexInElement = nStart;
	m_nEndIndexInElement = nEnd;
}

void  Run::SetStart(uint nStart)
{
	m_nStartIndexInElement = nStart;
}

void  Run::SetEnd(uint nEnd)
{
	m_nEndIndexInElement = nEnd;
}

void  Run::SetPosInLine(int x)
{
    m_xInLine = x;
}

uint  Run::GetWidth()
{
	return m_w;
}

int  Run::GetXInLine()
{
	return m_xInLine;
}

void  Run::Draw(HDC hDC, RECT* prcLine)
{
    RECT  rcRun = *prcLine;
    rcRun.left += prcLine->left + m_xInLine;
    rcRun.right = rcRun.left + m_w;

	m_pElement->Draw(
		hDC, 
		m_nStartIndexInElement,
		m_nEndIndexInElement,
		&rcRun);
}

Element*  Run::GetElement()
{
    return m_pElement;
}