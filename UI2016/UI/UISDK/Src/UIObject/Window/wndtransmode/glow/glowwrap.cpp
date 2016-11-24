#include "stdafx.h"
#include "glowwrap.h"
#include "Src/UIObject/Window/customwindow.h"
#include "Src/Util/DPI/dpihelper.h"
#include "Inc/Util/util.h"
#include "Src/UIEffect/blur/webkit/shadowblur.h"

GlowWindowWrap::GlowWindowWrap()
{
	
}

GlowWindowWrap::~GlowWindowWrap()
{
    Enable(false);
}

void  GlowWindowWrap::Init(ICustomWindow* pWnd)
{
    __super::Init(pWnd);

}

void  GlowWindowWrap::Enable(bool b)
{
	if (b)
	{
		m_leftGW.Create(m_pWindow->GetHWND(), GLOW_WINDOW_LEFT);
		m_topGW.Create(m_pWindow->GetHWND(), GLOW_WINDOW_TOP);
		m_rightGW.Create(m_pWindow->GetHWND(), GLOW_WINDOW_RIGHT);
		m_bottomGW.Create(m_pWindow->GetHWND(), GLOW_WINDOW_BOTTOM);
	}
	else
	{
        if (m_leftGW.m_hWnd)
            m_leftGW.Destroy();
        if (m_topGW.m_hWnd)
            m_topGW.Destroy();
        if (m_rightGW.m_hWnd)
            m_rightGW.Destroy();
        if (m_bottomGW.m_hWnd)
            m_bottomGW.Destroy();
	}
}

void  GlowWindowWrap::UpdateRgn()
{

}

BOOL UI::GlowWindowWrap::ProcessWindowMessage(
        HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID /*= 0*/)
{
    if (uMsg == WM_SIZE)
    {
        if (wParam == SIZE_MAXIMIZED)
        {
            Hide();
        }
        else if (wParam == SIZE_RESTORED)
        {
            if (IsWindowVisible(hWnd))
            {
                Show();
            }
        }
    }

    return FALSE;
}

void  GlowWindowWrap::Show()
{
    if (m_leftGW.m_hWnd && m_leftGW.IsWindowVisible())
        return;

    if (m_leftGW.m_hWnd)
        m_leftGW.ShowWindow(SW_SHOWNOACTIVATE);
    if (m_topGW.m_hWnd)
        m_topGW.ShowWindow(SW_SHOWNOACTIVATE);
    if (m_rightGW.m_hWnd)
        m_rightGW.ShowWindow(SW_SHOWNOACTIVATE);
    if (m_bottomGW.m_hWnd)
        m_bottomGW.ShowWindow(SW_SHOWNOACTIVATE);
}
void  GlowWindowWrap::Hide()
{
    if (m_leftGW.m_hWnd)
        m_leftGW.ShowWindow(SW_HIDE);
    if (m_topGW.m_hWnd)
        m_topGW.ShowWindow(SW_HIDE);
    if (m_rightGW.m_hWnd)
        m_rightGW.ShowWindow(SW_HIDE);
    if (m_bottomGW.m_hWnd)
        m_bottomGW.ShowWindow(SW_HIDE);
}

GlowWindow::GlowWindow()
{
	m_nGlowSize = UI::ScaleByDpi(10);
	m_color = RGB(155, 159, 185);
}

void  GlowWindow::OnSize(UINT nType, CSize size)
{
	m_buffer.Destroy();
	m_buffer.Create(size.cx, size.cy, 32, Image::createAlphaChannel);

	render();
}

// 手动绘制阴影
void  GlowWindow::render()
{

	HDC hDC = m_buffer.GetDC();

	RECT rc;
	::GetClientRect(m_hWnd, &rc);
	HPEN hPen = CreatePen(PS_SOLID, 1, m_color);
	HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);

	switch (m_eAlign)
	{
	case GLOW_WINDOW_LEFT:
		MoveToEx(hDC, rc.right - 1, 0 + m_nGlowSize, NULL);
		LineTo(hDC, rc.right - 1, rc.bottom - m_nGlowSize);
		break;

	case GLOW_WINDOW_RIGHT:
		MoveToEx(hDC, 0, 0 + m_nGlowSize, NULL);
		LineTo(hDC, 0, rc.bottom - m_nGlowSize);
		break;

	case GLOW_WINDOW_TOP:
		MoveToEx(hDC, 0 + m_nGlowSize, rc.bottom - 1, NULL);
		LineTo(hDC, rc.right - m_nGlowSize, rc.bottom - 1);
		break;

	case GLOW_WINDOW_BOTTOM:
		MoveToEx(hDC, 0 + m_nGlowSize, 0, NULL);
		LineTo(hDC, rc.right - m_nGlowSize, 0);
		break;

	default:
		break;
	}
	
	SelectObject(hDC, hOldPen);
	DeleteObject(hPen);


	Util::FixAlphaData  data = { 0 };
	data.hDC = hDC;
	data.eMode = Util::SET_ALPHA_255_IF_RGBA_ISNOT_0;
	data.bTopDownDib = false;
	Util::FixBitmapAlpha(&data);
	m_buffer.ReleaseDC();

	ShadowBlur(m_buffer.GetHBITMAP(), m_color, NULL, 4);

	commit();
}


void GlowWindow::commit()
{
	POINT ptMemDC = { 0, 0 };

	int nFlag = ULW_ALPHA;
	BLENDFUNCTION  bf;
	bf.BlendOp = AC_SRC_OVER;
	bf.AlphaFormat = AC_SRC_ALPHA;
	bf.BlendFlags = 0;
	bf.SourceConstantAlpha = (byte)255;

	RECT rc;
	::GetWindowRect(m_hWnd, &rc);
	SIZE sz = { RECTW(rc), RECTH(rc) };

	HDC hDC = m_buffer.GetDC();
	::UpdateLayeredWindow(m_hWnd, NULL, NULL, &sz, hDC, &ptMemDC, 0, &bf, nFlag);
	m_buffer.ReleaseDC();
}

//
// Owner窗口:  只有重叠窗口和弹出窗口才能是Owner窗口, 子窗口不能为Owner窗口, 
//            Owner窗口销毁前, 他的所有的Owned窗口都将被自动销毁,当Owner
//            窗口隐藏时他的所有Owned窗口不会隐藏。但当Owner最小化是他的Owned
//            窗口会被隐藏。一个Owner窗口的所有Owned窗口都将在Owner窗口的
//            前面显示, 而不会在其后面显示。
//
// Owned窗口:  一个Owned窗口总是在Z - Order顺序中一般是在他的Owner窗口之前,
//            Owned窗口的生命可以被他的Owner窗口控制, Owned窗口的显示并不局
//            限于他的Owner窗口区域, 一个Owned窗口在建立后不能改变他的owner
//            窗口, 子窗口, 弹出窗口, 重叠窗口都可以做Owned窗口。
//
//
void  GlowWindow::Create(HWND hWnd, GLOW_WINDOW_ALIGN eAlign)
{
	UIASSERT(hWnd);
	UIASSERT(!m_hWnd);

	m_eAlign = eAlign;

	RECT rc = { 0 };
	::GetWindowRect(hWnd, &rc);

	switch (eAlign)
	{
	case GLOW_WINDOW_LEFT:
	{
		rc.right = rc.left;
		rc.left -= m_nGlowSize;
		rc.bottom += m_nGlowSize;
		rc.top -= m_nGlowSize;
	}
	break;

	case GLOW_WINDOW_TOP:
	{
		rc.bottom = rc.top;
		rc.top -= m_nGlowSize;
		rc.right += m_nGlowSize;
		rc.left -= m_nGlowSize;
	}
	break;

	case GLOW_WINDOW_RIGHT:
	{
		rc.left = rc.right;
		rc.right += m_nGlowSize;
		rc.bottom += m_nGlowSize;
		rc.top -= m_nGlowSize;
	}
	break;

	case GLOW_WINDOW_BOTTOM:
	{
		rc.top = rc.bottom;
		rc.bottom += m_nGlowSize;
		rc.right += m_nGlowSize;
		rc.left -= m_nGlowSize;
	}
	break;
	}

    long lStyle = WS_POPUP;
    //if (hWnd && ::IsWindowVisible(hWnd))
    //    lStyle |= WS_VISIBLE;

	// 先使用这个做为父窗口，解决同步激活的问题。
    __super::Create(hWnd/*NULL*/, &rc, NULL, lStyle,
        WS_EX_TOOLWINDOW | WS_EX_LAYERED /*| WS_EX_TRANSPARENT*/);
	UIASSERT(m_hWnd);

    

	SyncWindowData data;
	data.m_hWnd = m_hWnd;
	data.m_bAnchorOn = true;
    data.m_bSyncVisible = true;
	data.m_nMask = SWDS_MASK_ALL;

	switch (eAlign)
	{
	case GLOW_WINDOW_LEFT:
	{
		data.m_nAnchorType = ANCHOR_OUT_LEFT | ANCHOR_TOP | ANCHOR_BOTTOM;
		data.m_rcAnchorData.Width = m_nGlowSize;
		data.m_rcAnchorData.yOffset = -m_nGlowSize;
		data.m_rcAnchorData.yOffset2 = -m_nGlowSize;
	}
	break;
	case GLOW_WINDOW_RIGHT:
	{
		data.m_nAnchorType = ANCHOR_OUT_RIGHT | ANCHOR_TOP | ANCHOR_BOTTOM;
		data.m_rcAnchorData.Width = m_nGlowSize;
		data.m_rcAnchorData.yOffset = -m_nGlowSize;
		data.m_rcAnchorData.yOffset2 = -m_nGlowSize;
	}
	break;
	case GLOW_WINDOW_TOP:
	{
		data.m_nAnchorType = ANCHOR_OUT_TOP | ANCHOR_LEFT | ANCHOR_RIGHT;
		data.m_rcAnchorData.Height = m_nGlowSize;
		data.m_rcAnchorData.xOffset = -m_nGlowSize;
		data.m_rcAnchorData.xOffset2 = -m_nGlowSize;
	}
	break;
	case GLOW_WINDOW_BOTTOM:
	{
		data.m_nAnchorType = ANCHOR_OUT_BOTTOM | ANCHOR_LEFT | ANCHOR_RIGHT;
		data.m_rcAnchorData.Height = m_nGlowSize;
		data.m_rcAnchorData.xOffset = -m_nGlowSize;
		data.m_rcAnchorData.xOffset2 = -m_nGlowSize;
	}
	break;
	}
	
	SendMessage(hWnd, UI_WM_SYNC_WINDOW, ADD_SYNC_WINDOW, (LPARAM)&data);
}

void  GlowWindow::Destroy()
{
	__super::DestroyWindow();
	m_buffer.Destroy();
}

