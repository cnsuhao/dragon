#include "stdafx.h"
#include "gdirender.h"
#include "UISDK\Kernel\Src\UIEffect\blur\webkit\shadowblur.h"
#include "UISDK\Kernel\Src\UIEffect\CacheBitmap\cachebitmap.h"
#include "UISDK\Kernel\Src\Renderlibrary\gdiplus\gdiplusrender.h"
#include "UISDK\Kernel\Src\Renderlibrary\renderbuffer.h"
#include <math.h>


GdiRenderTarget::GdiRenderTarget()
{
    m_hBindDC = NULL;
	m_lDrawingRef = 0;
    m_pRenderBuffer = NULL;
#ifdef _DEBUG
    m_ptOffset.x = m_ptOffset.y = 0;
#endif
}
GdiRenderTarget::~GdiRenderTarget()
{
    SAFE_DELETE(m_pRenderBuffer);
 	m_hBindDC = NULL;
}

void  GdiRenderTarget::Release()
{
    delete this;
}

// 该HDC不需要释放 
HDC GdiRenderTarget::GetBindHDC()
{
	return m_hBindDC;
}

HRGN GdiRenderTarget::GetClipRgn()
{
	HRGN hRgn = ::CreateRectRgn(0,0,0,0);
	if( 1 != ::GetClipRgn(GetHDC(), hRgn) )  // 空或者失败
	{
		::DeleteObject(hRgn);
		hRgn = NULL;
	}
	return hRgn;
}
int GdiRenderTarget::SelectClipRgn( HRGN hRgn, int nMode )
{
	if (RGN_COPY == nMode/* && NULL != hRgn*/)
	{
		return ::SelectClipRgn(GetHDC(), hRgn);
	}

	int nRet = ExtSelectClipRgn(GetHDC(), hRgn, nMode);
	return nRet;
}

BOOL GdiRenderTarget::GetViewportOrgEx( LPPOINT lpPoint )
{
	if (NULL == lpPoint)
	{
		return FALSE;
	}

	::GetViewportOrgEx(GetHDC(), lpPoint );
	return TRUE;
}
BOOL GdiRenderTarget::SetViewportOrgEx( int x, int y, LPPOINT lpPoint ) 
{
	BOOL bRet1 = ::SetViewportOrgEx(GetHDC(), x, y, lpPoint);

#ifdef _DEBUG
    ::GetViewportOrgEx(GetHDC(), &m_ptOffset);
#endif
	return bRet1;
}
BOOL GdiRenderTarget::OffsetViewportOrgEx(int x, int y, LPPOINT lpPoint)
{
	BOOL bRet1 = ::OffsetViewportOrgEx(GetHDC(), x, y, lpPoint );

#ifdef _DEBUG
    ::GetViewportOrgEx(GetHDC(), &m_ptOffset);
#endif
	return bRet1;
}

void GdiRenderTarget::BindHDC(HDC hDC)
{
	if (NULL == hDC)
		return;

    if (m_pRenderBuffer)
    {
        SAFE_DELETE(m_pRenderBuffer);
    }
	m_hBindDC = hDC;
}

bool  GdiRenderTarget::CreateRenderBuffer(IRenderTarget*  pSrcRT)
{
    if (m_pRenderBuffer)
        return false;

    m_pRenderBuffer = new RenderBuffer;
    m_hBindDC = NULL;

    return true;
}

bool  GdiRenderTarget::ResizeRenderBuffer(unsigned int nWidth, unsigned int nHeight)
{
    if (!m_pRenderBuffer)
        CreateRenderBuffer(NULL);

    m_pRenderBuffer->Resize(nWidth, nHeight);
    return true;
}

void  GdiRenderTarget::GetRenderBufferData(ImageData*  pData)
{
	if (!m_pRenderBuffer)
		return;
	m_pRenderBuffer->GetImageData(pData);
}

HDC   GdiRenderTarget::GetHDC()
{
    if (m_hBindDC)
        return m_hBindDC;
    
    if (m_pRenderBuffer)
        return m_pRenderBuffer->m_hDC;

    UIASSERT(0);
    return NULL;
}
void  GdiRenderTarget::ReleaseHDC(HDC hDC)
{

}

//
// 如果需要同时绘制两个item项，则可以提供两个RECT进行裁剪
//
// bClear主要是用于分层窗口中的透明背景
//
bool GdiRenderTarget::BeginDraw()
{
    UIASSERT (0 == m_lDrawingRef);
    m_lDrawingRef ++;
    return true;
}

void GdiRenderTarget::EndDraw()
{
    -- m_lDrawingRef;
	if (m_lDrawingRef==0)
	{
		this->SetViewportOrgEx(0,0);
		this->SelectClipRgn(NULL);
	}
}

void GdiRenderTarget::Clear(DWORD dwColor, RECT* prc)
{
    HDC hDC = GetHDC();

    HBRUSH hBrush = ::CreateSolidBrush(dwColor);
    if (prc)
    {
        ::FillRect(hDC, prc, hBrush);
    }
    else
    {
        BITMAP  bm;
        HBITMAP hBitmap = (HBITMAP)::GetCurrentObject(hDC, OBJ_BITMAP);
        if (NULL == hBitmap)
            return;

        ::GetObject(hBitmap, sizeof(bm), &bm);
        RECT  rc = {0,0, bm.bmWidth, bm.bmHeight};
        ::FillRect(hDC, &rc, hBrush);
    }
    SAFE_DELETE_GDIOBJECT(hBrush);
}

void  GdiRenderTarget::DrawString(IRenderFont* pRenderFont, DRAWTEXTPARAM* pParam)
{
    if (NULL == pParam || NULL == pRenderFont)
        return;

	if (NULL == pParam->prc || pParam->prc->Width() <= 0 || pParam->prc->Height() <= 0)
		return;
	

    HDC hDC = GetHDC();
    GRAPHICS_RENDER_LIBRARY_TYPE  eType = pRenderFont->GetGraphicsRenderLibraryType();
	if (eType != GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
	{
        if (eType == GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS)
        {
            GdiplusRenderTarget::DrawStringEx(hDC, pRenderFont, pParam);
            return;
        }
        else
        {
		    UI_LOG_WARN(_T("GdiRenderTarget::DrawString pRenderFont render type != GRAPHICS_RENDER_LIBRARY_TYPE_GDI"));
	    	return;
        }
	}

    switch (pParam->nEffectFlag)
    {
    case TEXT_EFFECT_NONE:
        {
            HFONT hOldFont = (HFONT)::SelectObject(hDC, ((GDIRenderFont*)pRenderFont)->GetHFONT());

            COLORREF col = pParam->color.GetGDICompatibleValue();
            COLORREF oldCol = ::SetTextColor(hDC, col);

            ::DrawText(hDC, pParam->szText, _tcslen(pParam->szText), (RECT*)pParam->prc, pParam->nFormatFlag);

            ::SetTextColor(hDC,oldCol);
            ::SelectObject(hDC, hOldFont);
        }
        break;

    case TEXT_EFFECT_HALO:
        {
            HDC hMemDC = CreateCompatibleDC(NULL);
            SetBkMode(hMemDC, TRANSPARENT);

            int nWidth = pParam->prc->Width();
            int nHeight = pParam->prc->Height();
            RECT rcMem = { 0, 0, pParam->prc->Width(), pParam->prc->Height() };

            HBITMAP hMemBmp = CacheBitmap::GetInstance()->Create(nWidth, nHeight);
            HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hMemBmp);

            // 先画上背景，因为默认的DC图片是全0
            ::BitBlt(hMemDC, 0, 0, pParam->prc->Width(), pParam->prc->Height(), 
                hDC, pParam->prc->left, pParam->prc->top, SRCCOPY);

            HFONT hOldFont = (HFONT)::SelectObject(hMemDC, ((GDIRenderFont*)pRenderFont)->GetHFONT());
            COLORREF colorShadow = pParam->bkcolor.GetGDICompatibleValue();
            COLORREF oldColor = ::SetTextColor(hMemDC, colorShadow);

            // 阴影
            ::DrawText(hMemDC, pParam->szText, _tcslen(pParam->szText), (RECT*)&rcMem, pParam->nFormatFlag);

            // 模糊
            RECT rc = {0, 0, nWidth, nHeight};
//             recursive_blur<> blur;
//             blur.blur(hMemBmp, (double)pParam->wParam, &rc, 0);
            ShadowBlur(hMemBmp, pParam->bkcolor.GetGDICompatibleValue(), &rc, pParam->wParam);

            // 文字
            COLORREF color = pParam->color.GetGDICompatibleValue();
            ::SetTextColor(hMemDC, color);
            ::DrawText(hMemDC, pParam->szText, _tcslen(pParam->szText), (RECT*)&rcMem, pParam->nFormatFlag);

            ::BitBlt(hDC, pParam->prc->left, pParam->prc->top, pParam->prc->Width(), pParam->prc->Height(), hMemDC, 0, 0, SRCCOPY);
            ::SetTextColor(hMemDC, oldColor);
            ::SelectObject(hMemDC, hOldFont);
            ::SelectObject(hMemDC, hOldBmp);
            ::DeleteDC(hMemDC);
        }
        break;
    }
}



void GdiRenderTarget::FillRgn( HRGN hRgn, UI::Color* pColor)
{
    if (NULL == pColor)
        return;

	HBRUSH hBrush = ::CreateSolidBrush(RGB(pColor->r, pColor->g, pColor->b));
	::FillRgn(GetHDC(), hRgn, hBrush);
	::DeleteObject(hBrush);
}

void GdiRenderTarget::FillRect(const RECT* lprc, UI::Color* pColor)
{
    if (NULL == pColor)
        return;

    COLORREF gdicolor = 0;
    if (0 == pColor->a)
        return;
    
    gdicolor = pColor->GetGDICompatibleValue();

	HBRUSH hBrush = ::CreateSolidBrush(gdicolor);
	int n = ::FillRect(GetHDC(), lprc, hBrush);
	::DeleteObject(hBrush);
}

void GdiRenderTarget::TileRect(const CRect* lprc, IRenderBitmap* pRenderBitmap)
{
	if (NULL == pRenderBitmap)
		return;

	IRenderBitmap* p = (IRenderBitmap*)pRenderBitmap;
	if (p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
		return;

	GDIRenderBitmap* pBitmap = (GDIRenderBitmap*)p;
	HBRUSH hBrush = ::CreatePatternBrush(pBitmap->GetBitmap()->operator HBITMAP());
	::FillRect(GetHDC(), lprc, hBrush);
	::DeleteObject(hBrush);
}

//
// 注：由于控件绘制时已经被裁剪了区域，这将导致使用pen绘制边框时，有一部分绘制出来的
//     边框位于控件外面而被剪裁。
//
void GdiRenderTarget::Rectangle(const CRect* lprc, UI::Color* pColBorder, UI::Color* pColBack, int nBorder, bool bNullBack)
{
	COLORREF gdicolBorder = 0;
	COLORREF gdicolBack = 0;
	
    if (!pColBorder)
        return;

    if (0 == pColBorder->a)
        return;

    gdicolBorder = pColBorder->GetGDICompatibleValue();

	if (!bNullBack && pColBack && 0 != pColBack->a)
	{
        gdicolBack = pColBack->GetGDICompatibleValue();
	}

	HPEN hPen = ::CreatePen(PS_SOLID, nBorder, gdicolBorder);
	HBRUSH hBrush = NULL;
	if (bNullBack)
	{
		hBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
	}
	else
	{
		hBrush = ::CreateSolidBrush(gdicolBack);
	}

	HDC hDC = GetHDC();

	HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, hBrush);

	::Rectangle(hDC, lprc->left, lprc->top, lprc->right, lprc->bottom);

	::SelectObject(hDC, hOldPen);
	::SelectObject(hDC, hOldBrush);
	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}

void GdiRenderTarget::DrawFocusRect( const CRect* lprc )
{
	::DrawFocusRect(GetHDC(), lprc);
}
void GdiRenderTarget::DrawLine(int x1, int y1, int x2, int y2, IRenderPen* pPen)
{
	if (NULL == pPen)
		return;

	if (pPen->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
		return;

    HDC hDC = GetHDC();
	GdiPen* pGdiPen = (GdiPen*)pPen;
	HPEN hOldPen = (HPEN)::SelectObject(hDC, pGdiPen->m_hPen);

	::MoveToEx(hDC, x1, y1, NULL);
	::LineTo(hDC, x2, y2);

	::SelectObject(hDC, hOldPen);
}
void GdiRenderTarget::DrawPolyline(POINT* lppt, int nCount, IRenderPen* pPen)
{
	if (NULL == pPen || NULL == lppt)
		return;

	if (pPen->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
		return;

    HDC hDC = GetHDC();

	GdiPen* pGdiPen = (GdiPen*)pPen;
	HPEN hOldPen = (HPEN)::SelectObject(hDC, pGdiPen->m_hPen);
	::Polyline(hDC, lppt, nCount);
	::SelectObject(hDC, hOldPen);
}

void GdiRenderTarget::GradientFillH( const CRect* lprc, COLORREF colFrom, COLORREF colTo )
{
	Util::GradientFillH(GetHDC(), lprc, colFrom, colTo );
}
void GdiRenderTarget::GradientFillV( const CRect* lprc, COLORREF colFrom, COLORREF colTo )
{
	Util::GradientFillV(GetHDC(), lprc, colFrom, colTo );
}

void GdiRenderTarget::BitBlt(int xDest, int yDest, int wDest, int hDest, IRenderTarget* pSrcHDC, int xSrc, int ySrc, DWORD dwRop)
{
	if (NULL == pSrcHDC)
		return;
	if (pSrcHDC->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
		return;

    GdiRenderTarget*  pSrcRT = static_cast<GdiRenderTarget*>(pSrcHDC);
	::BitBlt(GetHDC(), xDest,yDest,wDest,hDest, pSrcRT->GetHDC(),xSrc,ySrc,dwRop);
}

void GdiRenderTarget::DrawBitmap( IRenderBitmap* hBitmap, int x, int y)
{
	if (NULL == hBitmap)
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if (p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
		return;

	GDIRenderBitmap* pBitmap = (GDIRenderBitmap*)p;
	if (NULL == pBitmap)
		return;

	Image* pImage = pBitmap->GetBitmap();
	if (NULL == pImage)
		return;

	pImage->Draw(GetHDC(), x,y);
}
void GdiRenderTarget::DrawBitmap(IRenderBitmap* pBitmap, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc)
{
	if (NULL == pBitmap)
		return;

	if (pBitmap->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
		return;

	GDIRenderBitmap* pRenderBitmap = static_cast<GDIRenderBitmap*>(pBitmap);
	if (NULL == pBitmap)
		return;

	Image* pImage = pRenderBitmap->GetBitmap();
	if (NULL == pImage)
		return;

	pImage->Draw(GetHDC(), xDest,yDest, wDest,hDest, xSrc, ySrc, wDest, hDest);
}
void GdiRenderTarget::DrawBitmap( IRenderBitmap* hBitmap, int xDest, int yDest, int nDestWidth, 
							int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight )
{
	if( NULL == hBitmap )
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if( p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI )
		return;

	GDIRenderBitmap* pBitmap = (GDIRenderBitmap*)p;
	if (NULL == pBitmap)
		return;

	Image* pImage = pBitmap->GetBitmap();
	if (NULL == pImage)
		return;

	pImage->Draw(GetHDC(), xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, nSrcWidth, nSrcHeight );
}

void GdiRenderTarget::DrawBitmap( IRenderBitmap* hBitmap, int xDest, int yDest, int nDestWidth, 
					int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
					C9Region* p9Region )
{
	if( NULL == hBitmap )
		return;

	if( NULL == p9Region )
		return this->DrawBitmap( hBitmap, xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, nSrcWidth, nSrcHeight );

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if( p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI )
		return;

	GDIRenderBitmap* pBitmap = (GDIRenderBitmap*)p;
	if (NULL == pBitmap)
		return;

	Image* pImage = pBitmap->GetBitmap();
	if (NULL == pImage)
		return;

	pImage->Draw(GetHDC(), xDest, yDest, nDestWidth, nDestHeight, xSrc, ySrc, nSrcWidth, nSrcHeight, p9Region );
}

void GdiRenderTarget::ImageList_Draw( IRenderBitmap* hBitmap, int x, int y, int col, int row, int cx, int cy )
{
	if( NULL == hBitmap )
		return;

	IRenderBitmap* p = (IRenderBitmap*)hBitmap;
	if( p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI )
		return;

	GDIRenderBitmap* pBitmap = (GDIRenderBitmap*)p;
	if (NULL == pBitmap)
		return;

	Image* pImage = pBitmap->GetBitmap();
	if (NULL == pImage)
		return;

	pImage->ImageList_Draw(GetHDC(), x,y,col,row,cx,cy);
}

void GdiRenderTarget::DrawBitmapEx(HDC hDC, IRenderBitmap* pBitmap, DRAWBITMAPPARAM* pParam)
{
	IRenderBitmap* p = (IRenderBitmap*)pBitmap;
	if (p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
	{
		if (p->GetGraphicsRenderLibraryType() == GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS) // 需要抗锯齿的情况
		{
			GdiplusRenderTarget::DrawBitmapEx(hDC, p, pParam);
		}
		return;
	}

	Image* pImage =  ((GDIRenderBitmap*)p)->GetBitmap();
	Image  imageDisable;
	if (pParam->nFlag & DRAW_BITMAP_DISABLE)
	{
		imageDisable.CopyGrayImageFrom(pImage);
		pImage = &imageDisable;
	}

	if (pParam->nFlag & DRAW_BITMAP_STRETCH)
	{
		pImage->Draw(hDC, pParam->xDest, pParam->yDest, pParam->wDest, pParam->hDest, pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc, pParam->pRegion, true, pParam->nAlpha);
        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(pParam->xDest, pParam->yDest, pParam->xDest+pParam->wDest, pParam->yDest+pParam->hDest);
        }
	}
    else if (pParam->nFlag & DRAW_BITMAP_BITBLT)
    {
        int nW = min(pParam->wDest, pParam->wSrc);
        int nH = min(pParam->hDest, pParam->hSrc);
        pImage->Draw(hDC, pParam->xDest, pParam->yDest, nW, nH, pParam->xSrc, pParam->ySrc, nW, nH, pParam->nAlpha);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(pParam->xDest, pParam->yDest, pParam->xDest+nW, pParam->yDest+nH);
        }
    }
	else if (pParam->nFlag & DRAW_BITMAP_STRETCH_BORDER)
	{
		pImage->Draw(hDC, pParam->xDest, pParam->yDest, pParam->wDest, pParam->hDest, pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc, pParam->pRegion, false, pParam->nAlpha);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(pParam->xDest, pParam->yDest, pParam->xDest+pParam->wDest, pParam->yDest+pParam->hDest);
        }
	}
	else if (pParam->nFlag & DRAW_BITMAP_TILE)
	{
		if (NULL == pBitmap)
			return;

		RECT rc = {pParam->xDest, pParam->yDest, pParam->xDest+pParam->wDest, pParam->yDest+pParam->hDest};
		HBRUSH hBrush = ::CreatePatternBrush(pImage->operator HBITMAP());
		::FillRect(hDC, &rc, hBrush);
		::DeleteObject(hBrush);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(pParam->xDest, pParam->yDest, pParam->xDest+pParam->wDest, pParam->yDest+pParam->hDest);
        }
	}
	else if (pParam->nFlag & DRAW_BITMAP_CENTER)
	{
		int x = pParam->xDest + (pParam->wDest - pParam->wSrc)/2;
		int y = pParam->yDest + (pParam->hDest - pParam->hSrc)/2;

		pImage->Draw(hDC, x,y, pParam->wSrc, pParam->hSrc, pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc, pParam->nAlpha);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(x, y, x+pParam->wSrc, y+pParam->hSrc);
        }
	}
	else if (pParam->nFlag & DRAW_BITMAP_ADAPT)
	{
		if (pParam->wSrc == 0 || pParam->hSrc == 0)
			return;

		if (pParam->wDest == 0 || pParam->hDest == 0)
			return;


		bool bNeedToStretch = false;
		int  wImage = pParam->wSrc;
		int  hImage = pParam->hSrc;

		if (pParam->wDest < pParam->wSrc || pParam->hDest < pParam->hSrc)
		{
			bNeedToStretch = true;

			double tan_x_y_image = (double)pParam->wSrc / (double)pParam->hSrc;
			double tan_x_y_dest = (double)pParam->wDest / (double)pParam->hDest;

			if( tan_x_y_image > tan_x_y_dest ) // 横向占满
			{
				wImage = pParam->wDest;
				hImage = (int)((double)wImage/tan_x_y_image);
			}
			else   // 纵向占满
			{
				hImage = pParam->hDest;
				wImage = (int)(hImage*tan_x_y_image);
			}
		}

		// 计算图片显示位置
		int xDisplayPos = pParam->xDest + (pParam->wDest-wImage)/2;
		int yDisplayPos = pParam->yDest + (pParam->hDest-hImage)/2;

		if (bNeedToStretch)
		{
			pImage->Draw(hDC, xDisplayPos, yDisplayPos, wImage, hImage, pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc, pParam->pRegion, true, pParam->nAlpha);
		}
		else
		{
			pImage->Draw(hDC, xDisplayPos, yDisplayPos, wImage, hImage, pParam->xSrc, pParam->ySrc, wImage, hImage, pParam->nAlpha);
		}

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(xDisplayPos, yDisplayPos, xDisplayPos+wImage, yDisplayPos+hImage);
        }
	}
    else if (pParam->nFlag & DRAW_BITMAP_BITBLT_RIGHTTOP)
    {
        int nW = min(pParam->wDest, pParam->wSrc);
        int nH = min(pParam->hDest, pParam->hSrc);

        int xDest = pParam->xDest + (pParam->wDest-nW);
        int xSrc = pParam->xSrc+ (pParam->wSrc-nW);

        pImage->Draw(hDC, xDest, pParam->yDest, nW, nH, xSrc, pParam->ySrc, nW, nH, pParam->nAlpha);

        if (pParam->prcRealDraw)
        {
            pParam->prcRealDraw->SetRect(xDest, pParam->yDest, xDest+nW, pParam->yDest+nH);
        }
    }
}
void GdiRenderTarget::DrawBitmap(IRenderBitmap* pRenderBitmap, DRAWBITMAPPARAM* pParam)
{
	if (NULL == pRenderBitmap || NULL == pParam)
		return;

	GdiRenderTarget::DrawBitmapEx(GetHDC(), pRenderBitmap, pParam);
}

#if 0
//
//
// 绕着图片中心旋转nDegree度绘制图片
// 
//
// 使用SetWorldTransform也可以实现旋转等效果，具体MSDN是有Example
//
// The SetWorldTransform function sets a two-dimensional linear transformation between 
// world space and page space for the specified device context. This transformation can 
// be used to scale, rotate, shear, or translate graphics output.
//
// 点阵转换公式 
//  newx = x * eM11 + y * eM21 + eDx 
//  newy = x * eM12 + y * eM22 + eDy 
//  其中eM11和eM22是角度的余弦值，eM21是角度的正弦，eM12是eM21的负值。
//
// [注1] 如果hDC是一个MemDC的话，则必须在绘制完之后调用ModifyWorldTransform(hMemDC, NULL, MWT_IDENTITY);
//       否则该memdc提交到窗口DC上的内容将不正确
// [注2] 默认这些旋转都是基本(0,0)坐标的，但该函数要实现的目标是基本自身中心旋转
//
void GdiRenderTarget::DrawRotateBitmap(IRenderBitmap* pBitmap, int nDegree, DRAWBITMAPPARAM* pParam)
{

	IRenderBitmap* p = (IRenderBitmap*)pBitmap;
	if (p->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
		return;

#define M_PI 3.14159265358979323846

	Image* pImage =  ((GDIRenderBitmap*)p)->GetBitmap();
	if (pImage)
	{
		POINT ptCenter = {pParam->xDest+pParam->wDest/2, pParam->yDest+pParam->hDest/2};  // 图片目标区域的中心，绕该点旋转，而不是默认是（0，0）坐标旋转

		double fAngle =  M_PI * nDegree / 180; // (2*pi *nDegree)/360 ; 
		float cosAngle = (float)cos(fAngle);
		float sinAngle = (float)sin(fAngle);

#if 0  // 使用PlgBlt绘制的图片将无法使用Alpha，因此放弃

		POINT ptOri[3] = // 目标绘制坐标（未旋转前）
		{
			{pParam->xDest, pParam->yDest}, 
			{pParam->xDest+pParam->wDest, pParam->yDest}, 
			{pParam->xDest, pParam->yDest+pParam->hDest}
		};
		POINT ptOriToCenter[3] = // 相对于图片中心位置
		{
			{ptOri[0].x - ptCenter.x, ptOri[0].y - ptCenter.y},
			{ptOri[1].x - ptCenter.x, ptOri[1].y - ptCenter.y},
			{ptOri[2].x - ptCenter.x, ptOri[2].y - ptCenter.y}
		};

		POINT ptArg[3] = {0};  // 计算旋转后的平行四边行的前三个坐标位置
		for (int i = 0; i < 3; i++)
		{
			ptArg[i].x = (int)(ptOriToCenter[i].x*cosAngle + ptOriToCenter[i].y*sinAngle); 
			ptArg[i].y = (int)(ptOriToCenter[i].y*cosAngle - ptOriToCenter[i].x*sinAngle);

			// 恢复为相对于（0，0）坐标
			ptArg[i].x += ptCenter.x;
			ptArg[i].y += ptCenter.y;
		}

		pImage->PlgBlt(m_hDC, ptArg, pParam->xSrc, pParam->ySrc, pParam->wSrc, pParam->hSrc, NULL, 0, 0);

#else
        HDC hDC = GetHDC();

 		int nOldGraphicsMode = ::GetGraphicsMode(m_hDC);
 		::SetGraphicsMode(hDC, GM_ADVANCED);
  		XFORM xForm = {0};
		xForm.eM11 = cosAngle;
		xForm.eM12 = sinAngle;
		xForm.eM21 = -sinAngle;
		xForm.eM22 = cosAngle;
		xForm.eDx = ptCenter.x - cosAngle*ptCenter.x + sinAngle*ptCenter.y;
		xForm.eDy = ptCenter.y - cosAngle*ptCenter.y - sinAngle*ptCenter.x;
		SetWorldTransform(hDC, &xForm);
 		DrawBitmapEx(hDC, pBitmap, pParam);

		ModifyWorldTransform(hDC, NULL, MWT_IDENTITY);
 		::SetGraphicsMode(hDC, nOldGraphicsMode);
#endif
	}
}
#endif


IRenderPen* GdiRenderTarget::CreateSolidPen(int nWidth, Color* pColor)
{
	IRenderPen* p = NULL;
	GdiPen::CreateInstance(&p);

	if (p)
	{
		p->CreateSolidPen(nWidth, pColor);
	}
	return p;
}
IRenderPen* GdiRenderTarget::CreateDotPen(int nWidth, Color* pColor)
{
	IRenderPen* p = NULL;
	GdiPen::CreateInstance(&p);

	if (p)
	{
		p->CreateDotPen(nWidth, pColor);
	}
	return p;
}
IRenderBrush*  GdiRenderTarget::CreateSolidBrush(Color* pColor)
{
	IRenderBrush* p = NULL;
	GdiBrush::CreateInstance(&p);

	if (p)
	{
		p->CreateSolidBrush(pColor);
	}
	return p;
}

void  GdiRenderTarget::Render2DC(HDC hDstDC, Render2TargetParam* pParam)
{
    int& xDst = pParam->xDst;
    int& yDst = pParam->yDst; 
    int& wDst = pParam->wDst; 
    int& hDst = pParam->hDst; 
    int& xSrc = pParam->xSrc; 
    int& ySrc = pParam->ySrc; 
    int& wSrc = pParam->wSrc; 
    int& hSrc = pParam->hSrc; 
    bool& bAlphaBlend = pParam->bAlphaBlend;
    byte& opacity = pParam->opacity;

    HDC hDC = GetHDC();
    UIASSERT (hDC != hDstDC);

    int nOldGraphicsMode = 0;
    if (pParam->pTransform)
    {
        nOldGraphicsMode = ::GetGraphicsMode(hDstDC);
        ::SetGraphicsMode(hDstDC, GM_ADVANCED);
        ::SetWorldTransform(hDstDC, pParam->pTransform);
    }
    
    if (bAlphaBlend)
    {
        BLENDFUNCTION bf = {AC_SRC_OVER, 0, opacity, AC_SRC_ALPHA};
        ::AlphaBlend(hDstDC, xDst, yDst, wDst, hDst, hDC, xSrc, ySrc, wSrc, hSrc, bf);
    }
    else
    {
        if (wDst == wSrc && hDst == hSrc)
        {
            ::BitBlt(hDstDC, xDst, yDst, wDst, hDst, hDC, xSrc, ySrc, SRCCOPY);
        }
        else
        {
            ::StretchBlt(hDstDC, xDst, ySrc, wDst, hDst, hDC, xSrc, ySrc, wSrc, hSrc, SRCCOPY);
        }
    }
    
    if (pParam->pTransform)
    {
        ModifyWorldTransform(hDstDC, NULL, MWT_IDENTITY);
        ::SetGraphicsMode(hDstDC, nOldGraphicsMode);
    }
}
void  GdiRenderTarget::Render2Target(IRenderTarget* pDst, Render2TargetParam* pParam)
{
    if (!pDst)
        return;

    if (pDst->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
    {
        UI_LOG_WARN(_T("%s Graphcis Render library is wrong"), FUNC_NAME);
        return;
    }

    GdiRenderTarget* pGdiDst = static_cast<GdiRenderTarget*>(pDst);
    return Render2DC(pGdiDst->GetHDC(), pParam);
}

void  GdiRenderTarget::Save(const TCHAR*  szPath )
{
    if (m_pRenderBuffer)
    {
        m_pRenderBuffer->Dump();
    }
}