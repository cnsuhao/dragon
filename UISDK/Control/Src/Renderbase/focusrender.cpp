#include "stdafx.h"
#include "focusrender.h"

ListViewFocusRender::ListViewFocusRender()
{
	m_nDeflat = 3;
}

void  ListViewFocusRender::OnDrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;

    HDC hDC = pRenderTarget->GetHDC();
    HTHEME hTheme = this->GetTHEME();
    if (hTheme)
    {
        HRESULT hr = DrawThemeBackground(hTheme, hDC, 3, 1, (RECT*)&pDrawStruct->rc, 0);
        if (S_OK != hr)
        {
            UI_LOG_WARN(_T("DrawThemeBackground failed."));
        }
    }
    else
    {
        CRect rc (&pDrawStruct->rc);
        rc.DeflateRect(m_nDeflat, m_nDeflat);

        ::DrawFocusRect(hDC, &rc);
    }
}

