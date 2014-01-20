#include "stdafx.h"
#include "windowrenderlayet.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\RenderLayer\renderchain.h"

namespace UI{

void  WindowRenderLayer::Draw()
{
    if (!CanRedraw())
        return;

    if (NULL == m_pRenderTarget)
	{
        CRect rc;
        ::GetClientRect(m_pWindow->m_hWnd, &rc);
        CreateBuffer(rc.Width(), rc.Height());
// 
// #ifdef _DEBUG
// 		::MessageBox(NULL, _T("NULL == m_pRenderTarget, 是否是外部拦截了窗口WM_SIZE消息?"), _T("Error"), MB_OK|MB_ICONERROR);
// #endif
//        return;
	}

    if (m_pRenderTarget->BeginDraw(NULL, 0))
    {
        if (m_pWindow->IsTransparent())
            m_pRenderTarget->Clear();

        CRect  rc;
        m_pWindow->GetParentRect(&rc);
        RenderContext roc(&rc, true, m_pRenderChain->GetRequireAlphaChannel());

        m_pWindow->DrawObject(m_pRenderTarget, roc);
        m_pRenderTarget->EndDraw();
    }
}

}