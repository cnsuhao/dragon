#include "stdafx.h"
#include "windowrender.h"
#include "renderlayer2.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Inc\Interface\iwndtransmode.h"
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"
// http://www.chromium.org/developers/design-documents/gpu-accelerated-compositing-in-chrome

namespace UI
{
WindowRender::WindowRender(WindowBase*  p)
{
    m_eGRL = GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
    m_pWindow = p;
    m_lRefCanCommit = 0;
    m_bFirstTimePaintWindow = true;

    SetCanCommit(false);
    m_pIWindowRender = NULL;
}
WindowRender::~WindowRender()
{
    SAFE_DELETE(m_pIWindowRender);
}

IWindowRender*  WindowRender::GetIWindowRender()
{
    if (!m_pIWindowRender)
        m_pIWindowRender = new IWindowRender(this);

    return m_pIWindowRender;
}

void  WindowRender::ResetAttribute()
{
	m_eGRL = GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
}

GRAPHICS_RENDER_LIBRARY_TYPE  UIParseGraphicsRenderLibraryType(const TCHAR* szText);
void  WindowRender::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
	const TCHAR*  szText = pMapAttrib->GetAttr(XML_WINDOW_GRAPHICS_RENDER_LIBRARY, true);
	if (szText)
		m_eGRL = UIParseGraphicsRenderLibraryType(szText);
}

// IRenderTarget* 没有引用计数机制
// 但仍然采用Release进行释放（delete） 
bool  WindowRender::CreateRenderTarget(IRenderTarget** pp)
{
    if (!pp)
        return false;

    *pp = UICreateRenderTarget(m_pWindow->GetUIApplication(), m_eGRL);
    return true;
}

void  WindowRender::OnWindowTransparentModeChanged(IWndTransMode* pMode)
{
    if (pMode && pMode->RequireAlphaChannel())
    {
        if (m_eGRL == GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
        {
            m_eGRL = GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS;
        }
    }
}

void  WindowRender::OnWindowPaint(HDC hDC)
{
    if (m_bFirstTimePaintWindow)
    {
        m_bFirstTimePaintWindow = false;
        SetCanCommit(true);
    }

    CRect rcClient;
    ::GetClientRect(m_pWindow->m_hWnd, &rcClient);
	m_pWindow->GetRenderLayer2()->UpdateLayer(NULL);
    
    Commit(hDC, 0);
}

void  WindowRender::SetCanCommit(bool b)
{
    if (b)
        m_lRefCanCommit --;
    else
        m_lRefCanCommit ++;
}

bool  WindowRender::CanCommit() 
{ 
    return 0 == m_lRefCanCommit; 
}
void  WindowRender::Commit(HDC hDC, RECT* prc, int nCount)
{
    if (CanCommit())
    {
        m_pWindow->CommitDoubleBuffet2Window(hDC, prc, prc?nCount:0);
    }
}

void  WindowRender::SetGraphicsRenderType(GRAPHICS_RENDER_LIBRARY_TYPE  eTpye)
{
	// 仅在窗口创建之前设置有用
	m_eGRL = eTpye;
}
GRAPHICS_RENDER_LIBRARY_TYPE  WindowRender::GetGraphicsRenderType()
{
	return m_eGRL;
}
bool  WindowRender::GetRequireAlphaChannel()
{
	return m_eGRL==GRAPHICS_RENDER_LIBRARY_TYPE_GDI?false:true; 
}

}