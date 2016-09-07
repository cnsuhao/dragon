#include "stdafx.h"
#include "progressctrl.h"

namespace UI
{

ProgressBar::ProgressBar()
{
    m_pIProgressBar = NULL;

	m_nMin  = 0;
	m_nMax  = 100;
	m_nCur  = 0;
	m_nPage = 10;
	m_nLine = 1;
	m_eDirectionType = PROGRESSBAR_SCROLL_LEFT_2_RIGHT;
}

HRESULT  ProgressBar::FinalConstruct(IUIApplication* )
{
    DO_PARENT_PROCESS(IProgressBar, IControl);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

    if (m_pIProgressBar->GetUIApplication()->IsDesignMode())
    {
        SetPos(50, false);
    }
    return S_OK;
}
void  ProgressBar::ResetAttribute()
{
    DO_PARENT_PROCESS(IProgressBar, IControl);
    m_eDirectionType = PROGRESSBAR_SCROLL_LEFT_2_RIGHT;
}

void ProgressBar::SetAttribute(IMapAttribute* pMapAttr, bool bReload)
{
	DO_PARENT_PROCESS(IProgressBar, IControl);

	LPCTSTR szText = pMapAttr->GetAttr(XML_PROGRESSBAR_DIRECTION, true);
	if (szText)
	{
		if (0 == _tcscmp(szText, XML_PROGRESSBAR_DIRECTION_LEFTRIGHT))
		{
			m_eDirectionType = PROGRESSBAR_SCROLL_LEFT_2_RIGHT;
		}
		else if (0 == _tcscmp(szText, XML_PROGRESSBAR_DIRECTION_BOTTOMTOP))
		{
			m_eDirectionType = PROGRESSBAR_SCROLL_BOTTOM_2_TOP;
		}
		else if (0 == _tcscmp(szText, XML_PROGRESSBAR_DIRECTION_RIGHTLEFT))
		{
			m_eDirectionType = PROGRESSBAR_SCROLL_RIGHT_2_LEFT;
		}
		else if (0 == _tcscmp(szText, XML_PROGRESSBAR_DIRECTION_TOPBOTTOM))
		{
			m_eDirectionType = PROGRESSBAR_SCROLL_TOP_2_BOTTOM;
		}
		else
		{
			UI_LOG_WARN(_T("\"%s\" is invalid: %s"), XML_PROGRESSBAR_DIRECTION, szText);
		}
	}

    if (NULL == m_pIProgressBar->GetBkRender())
    {
        IRenderBase* p = NULL;
        m_pIProgressBar->GetUIApplication()->CreateRenderBase(RENDER_TYPE_THEME_PROGRESS_BKGND, m_pIProgressBar, &p);
        if (p)
        {
            SERIALIZEDATA data = {0};
            data.pUIApplication = m_pIProgressBar->GetUIApplication();
            data.pMapAttrib = pMapAttr;
            data.szPrefix = XML_ALIGN_BOTTOM;
            data.nFlags = SERIALIZEFLAG_LOAD|SERIALIZEFLAG_LOAD_ERASEATTR;
            p->Serialize(&data);

            m_pIProgressBar->SetBkgndRender(p);
            SAFE_RELEASE(p);
        }
    }
    if (NULL == m_pIProgressBar->GetForeRender())
    {
        IRenderBase* p = NULL;
        m_pIProgressBar->GetUIApplication()->CreateRenderBase(RENDER_TYPE_THEME_PROGRESS_FOREGND, m_pIProgressBar, &p);
        if (p)
        {
            SERIALIZEDATA data = {0};
            data.pUIApplication = m_pIProgressBar->GetUIApplication();
            data.pMapAttrib = pMapAttr;
            data.szPrefix = XML_ALIGN_BOTTOM;
            data.nFlags = SERIALIZEFLAG_LOAD|SERIALIZEFLAG_LOAD_ERASEATTR;
            p->Serialize(&data);

            m_pIProgressBar->SetForegndRender(p);
            SAFE_RELEASE(p);
        }
    }
}

void  ProgressBar::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;
    IRenderBase* p = m_pIProgressBar->GetBkRender();
	if(p)
    {
		*pSize = p->GetDesiredSize();
    }

    p = m_pIProgressBar->GetForeRender();
	if(p)
    {
		*pSize = p->GetDesiredSize();
    }
}

int ProgressBar::SetPos(int nPos, bool bUpdate)
{
	int oldPos = m_nCur;

	if (nPos >= m_nMax)
		nPos = m_nMax;

	if (nPos <= m_nMin)
		nPos = m_nMin;

	m_nCur = nPos;

	if (m_nCur != oldPos && bUpdate)
		m_pIProgressBar->UpdateObject();

	return oldPos;
}

int ProgressBar::SetPage(int nPage, bool bUpdate)
{
	int nOldPage = m_nPage;
	m_nPage = nPage;

	int nRange = GetRange();

	if (m_nPage < 0)
		m_nPage = 0;
	if (m_nPage > nRange)
		m_nPage = nRange;

	if (nOldPage != m_nPage && bUpdate)
		m_pIProgressBar->UpdateObject();

	return nOldPage;
}

void ProgressBar::SetRange(int nLower, int nUpper, bool bUpdate)
{
	m_nMax = nUpper;
	m_nMin = nLower;

	if( m_nCur > m_nMax )
		m_nCur = m_nMax;
	if (m_nCur < m_nMin)
		m_nCur = m_nMin;

	if (bUpdate)
		m_pIProgressBar->UpdateObject();
}

void ProgressBar::SetScrollInfo(LPCSCROLLINFO lpsi, bool bUpdate)
{
	if (NULL == lpsi || 0 == lpsi->fMask)
		return;

	if (lpsi->fMask & SIF_RANGE)
		this->SetRange(lpsi->nMin, lpsi->nMax, false);

	if (lpsi->fMask & SIF_POS)
		this->SetPos(lpsi->nPos, false);

	if (lpsi->fMask & SIF_PAGE)
		this->SetPage(lpsi->nPage, false);

	if (bUpdate)
		m_pIProgressBar->UpdateObject();
}

void ProgressBar::OnPaint(IRenderTarget* pDC)
{
	if (m_nMax == m_nMin)
		return ;

    IRenderBase* pForegndRender = m_pIProgressBar->GetForeRender();
	if (pForegndRender)
	{
		CRect rc;
		m_pIProgressBar->GetObjectClientRect(&rc);

		switch(m_eDirectionType)
		{
		case PROGRESSBAR_SCROLL_LEFT_2_RIGHT:
			{
				int nX = rc.Width() * m_nCur / (m_nMax-m_nMin);
				rc.right = nX;
			}
			break;

		case PROGRESSBAR_SCROLL_RIGHT_2_LEFT:
			{
				int nX = rc.Width() * m_nCur / (m_nMax-m_nMin);
				rc.left = rc.right - nX;
			}
			break;

		case PROGRESSBAR_SCROLL_TOP_2_BOTTOM:
			{
				int nY = rc.Height() * m_nCur / (m_nMax-m_nMin);
				rc.bottom = nY;
			}
			break;

		case PROGRESSBAR_SCROLL_BOTTOM_2_TOP:
			{
				int nY = rc.Height() * m_nCur / (m_nMax-m_nMin);
				rc.top = rc.bottom - nY;
			}
			break;
		}

		pForegndRender->DrawState(pDC, &rc, 0);
	}
}

}