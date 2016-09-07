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
		m_leftGW.Destroy();
		m_topGW.Destroy();
		m_rightGW.Destroy();
		m_bottomGW.Destroy();
	}
}

void  GlowWindowWrap::UpdateRgn()
{

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

	// 先使用这个做为父窗口，解决同步激活的问题。
	__super::Create(hWnd/*NULL*/, &rc, NULL, WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TRANSPARENT);
	UIASSERT(m_hWnd);

	SyncWindowData data;
	data.m_hWnd = m_hWnd;
	data.m_bAnchorOn = true;
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

