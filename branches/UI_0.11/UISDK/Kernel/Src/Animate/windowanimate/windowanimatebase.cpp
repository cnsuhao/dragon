#include "stdafx.h"
#include "windowanimatebase.h"
#include "UISDK\Kernel\Src\Animate\storyboard.h"
#include "UISDK\Kernel\Src\Animate\windowanimate\layeredanimatewindow.h"
#include "UISDK\Kernel\Src\Animate\animatemgr.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"

namespace UI
{

WindowAnimateBase::WindowAnimateBase()
{
    m_pSrcBits = NULL;
    m_nSrcPitch = 0;
    m_nSrcWndWidth = 0;
    m_nSrcWndHeight = 0;
    m_pWindow = NULL;
    m_pAnimateMgr = NULL;
	m_rcWindowInBuffer.SetRectEmpty();

    m_pLayeredWindow = NULL;
    m_pIWindowAnimateBase = NULL;

    m_nWndTranslateType = WINDOW_TRANSPARENT_TYPE_NORMAL;
    m_pStroyboard = NULL;
}
WindowAnimateBase::~WindowAnimateBase() 
{
    if (m_pLayeredWindow)
    {
        ILayeredAnimateWindow*  pI = m_pLayeredWindow->GetILayeredAnimateWindow();
        SAFE_DELETE_Ixxx(pI);
        m_pLayeredWindow = NULL;
    }
    SAFE_DELETE(m_pStroyboard);
};


void  WindowAnimateBase::UpdateWindowSourceImage()
{
    if (NULL == m_pWindow)
        return;
 
    if (m_nWndTranslateType & WINDOW_TRANSPARENT_TYPE_AERO || 
        m_nWndTranslateType & WINDOW_TRANSPARENT_TYPE_LAYERED)
        GetSrcBitmap_Layered();
    else
        GetSrcBitmap_Normal();
}

void WindowAnimateBase::SetAnimateWindow(IWindowBase* pWindow)
{
	if (NULL == pWindow)
		return;

    m_pWindow = pWindow->GetImpl();
    m_nWndTranslateType = UISendMessage(m_pWindow, UI_WM_GET_WINDOW_TRANSPARENT_MODE);
}
void  WindowAnimateBase::Initialize()
{
	CRect rcWndOrigin;
	::GetWindowRect(m_pWindow->GetHWND(), &rcWndOrigin);
	m_nSrcWndWidth  = rcWndOrigin.Width();
	m_nSrcWndHeight = rcWndOrigin.Height();

	// 获取开启动画时的窗口画面，使用静止画面进行动画演示
	this->UpdateWindowSourceImage();

	// 设置动画参数
    ILayeredAnimateWindow*  pILayeredAnimateWindow = NULL;
    ILayeredAnimateWindow::CreateInstance(
			m_pWindow->GetIUIApplication(),
			m_pWindow->GetISkinRes(),
			&pILayeredAnimateWindow);   

	m_pLayeredWindow = pILayeredAnimateWindow->GetImpl();
    m_pLayeredWindow->SetWindowAnimate(this);
	m_pLayeredWindow->Soul_Attach(m_pWindow);

	// 显示动画窗口 -- 不能显示。因为有可能当前为两个连续动画，第二个动画时窗口有可能已经
	//                 旋转了90度，你一显示原窗口就坏了
// 	HDC hMemDC = m_SrcImage.GetDC();
// 	::BitBlt(m_pLayeredWindow->m_hLayeredMemDC, 0,0, m_rcWndOrigin.Width(), m_rcWndOrigin.Height(), hMemDC, 0,0, SRCCOPY);
// 	m_SrcImage.ReleaseDC();
// 	m_pLayeredWindow->Commit2LayeredWindow();
}

// 注：有些时候，不仅仅是需要在动画开始时获取一下窗口快照，在动画过程中也得更新窗口快照
//     例如带flash背景的窗口翻转
void WindowAnimateBase::GetSrcBitmap_Layered()
{
    // TODO: 未考虑动画过程中窗口变大的情况
    if (m_SrcImage.IsNull())
    {
        int nSrcImageW = m_nSrcWndWidth;
        int nSrcImageH = m_nSrcWndHeight;
        m_SrcImage.Create(nSrcImageW, -nSrcImageH, 32, Image::createAlphaChannel);

        m_pSrcBits  = (BYTE*)m_SrcImage.GetBits();
        m_nSrcPitch = m_SrcImage.GetPitch();

		m_rcWindowInBuffer.SetRect(0, 0, m_nSrcWndWidth, m_nSrcWndHeight);
    }
	
	HDC hMemDC = m_SrcImage.GetDC();
    RECT rc = {0,0, m_nSrcWndWidth, m_nSrcWndHeight};
    m_pWindow->DrawMemBitmap(hMemDC, &rc, false);
	m_SrcImage.ReleaseDC();
}
void WindowAnimateBase::GetSrcBitmap_Normal()
{
	if (NULL == m_pWindow)
		return;

	int nSrcImageW = m_nSrcWndWidth;
	int nSrcImageH = m_nSrcWndHeight;

	if (nSrcImageH <= 0 || nSrcImageW <= 0)
		return;

    // TODO: 未考虑动画过程中窗口变大的情况
    if (m_SrcImage.IsNull())
    {
	    m_SrcImage.Create(nSrcImageW, -nSrcImageH, 32, Image::createAlphaChannel);
    
        m_pSrcBits  = (BYTE*)m_SrcImage.GetBits();
        m_nSrcPitch = m_SrcImage.GetPitch();

		m_rcWindowInBuffer.SetRect(0, 0, nSrcImageW, nSrcImageH);
    }

	HDC hMemDC = m_SrcImage.GetDC();
#if 0 // (_WIN32_WINNT >= 0x0501)
	::PrintWindow(m_pWindow->GetHWND(), hMemDC, 0); // 对于普通窗口，该语句会导致要用于显示动画的窗口显示出来
#else
    RECT rc = {0,0, m_nSrcWndWidth, m_nSrcWndHeight};
    m_pWindow->DrawMemBitmap(hMemDC, &rc, false);
	//::BitBlt( hMemDC, 0,0, rc.right-rc.left,rc.bottom-rc.top, hWindowDC, 0,0, SRCCOPY ); // <-- 当窗口被遮盖时无法截图
#endif
	m_SrcImage.ReleaseDC();

	// 获取当前窗口的异形区域，用于绘制分层窗口时的透明
	HRGN hRgnWindow = ::CreateRectRgn(0,0,0,0);
	HRGN hRgnTransparent = NULL;

	int nRet = GetWindowRgn(m_pWindow->GetHWND(), hRgnWindow);
	bool bFullWindow = false;
	if (NULLREGION == nRet || ERROR == nRet)
	{
		bFullWindow = true;
	}
	else
	{
		hRgnTransparent = ::CreateRectRgn(0,0, m_nSrcWndWidth, m_nSrcWndHeight);
		::CombineRgn(hRgnTransparent, hRgnTransparent, hRgnWindow,RGN_DIFF);
	}
	SAFE_DELETE_GDIOBJECT(hRgnWindow);

	// 获取位图数据   
	LPBYTE lpbits = (LPBYTE)m_SrcImage.GetBits();
	int    nPitch = m_SrcImage.GetPitch();

	// 通过遍历整个图片像素，将位于窗口区域外的像素alpha值设为全透明
	for (LONG y = 0; y < nSrcImageH ; y++)   
	{
		for (LONG x = 0; x < nSrcImageW; x++)      
		{
			if (bFullWindow || !::PtInRegion(hRgnTransparent, x, y))
			{
				((DWORD*)lpbits)[x] |= 0xFF000000;
			}
			else
			{
				((DWORD*)lpbits)[x] = 0;
			}
		}
		lpbits += nPitch;
	}

	SAFE_DELETE_GDIOBJECT(hRgnTransparent);
}

}
