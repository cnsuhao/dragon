#include "stdafx.h"
#include "d2drendertarget.h"
#include "UISDK\Project\UI3D\src\D2D\d2drenderbuffer.h"
#include "d2drenderbitmap.h"
#include "d2drenderfont.h"

namespace UI
{

Direct2DRenderTarget::Direct2DRenderTarget()
{
    m_pRenderTarget = NULL;
    m_hBindDC = NULL;
    m_hBindWnd = NULL;
    m_pSrcRenderTarget = NULL;
    m_lClipPushRef = 0;
    m_lDrawRef = 0;
    m_ptOffset.x = m_ptOffset.y = 0;	
	m_nClipRectCount = 0;
	m_pArrClipRect = NULL;
}

Direct2DRenderTarget::~Direct2DRenderTarget()
{
	m_nClipRectCount = 0;
	SAFE_ARRAY_DELETE(m_pArrClipRect);

    ReleaseDeviceDependentRes();
    ReleaseDeviceIndependRes();
}

void   Direct2DRenderTarget::ReleaseDeviceDependentRes()
{
    // TODO: 释放位图

    SAFE_RELEASE(m_pRenderTarget);
}
void   Direct2DRenderTarget::ReleaseDeviceIndependRes()
{

}
void Direct2DRenderTarget::Release()
{
    delete this;
}

void   Direct2DRenderTarget::ClearBindInfo()
{
    m_hBindDC = NULL;
    m_hBindWnd = NULL;
    m_pSrcRenderTarget = NULL;
}

bool  Direct2DRenderTarget::CreateRenderBuffer(IRenderTarget*  pSrcRT)
{
    if (!pSrcRT)
        return false;

    Direct2DRenderTarget* pD2DRT = static_cast<Direct2DRenderTarget*>(pSrcRT);
    if (!pD2DRT)
        return false;

    ClearBindInfo();
    m_pSrcRenderTarget = pD2DRT;

    return true;
}

bool  Direct2DRenderTarget::ResizeRenderBuffer(unsigned int nWidth, unsigned int nHeight)
{
    if (m_hBindWnd)
    {
		if (m_pRenderTarget)
		{
			static_cast<ID2D1HwndRenderTarget*>(m_pRenderTarget)->Resize(D2D1::SizeU(nWidth, nHeight));
		}
		else
		{
		//	CreateRenderTarget();
		}
    }
	else if (m_hBindDC)
	{
		if (m_pRenderTarget)
		{
			static_cast<ID2D1DCRenderTarget*>(m_pRenderTarget)->BindDC(m_hBindDC, NULL);			
		}
		else
		{
		//	CreateRenderTarget();
		}
	}
	else if (m_pSrcRenderTarget && m_pSrcRenderTarget->m_pRenderTarget)
	{
		SAFE_RELEASE(m_pRenderTarget);
		
		ID2D1BitmapRenderTarget*  pMemRT = NULL;
		HRESULT hr = m_pSrcRenderTarget->m_pRenderTarget->CreateCompatibleRenderTarget(
			&D2D1::SizeF((float)nWidth, (float)nHeight),
			NULL, NULL, 
			D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_NONE/*D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_GDI_COMPATIBLE*/,
			&pMemRT);
		m_pRenderTarget = static_cast<ID2D1RenderTarget*>(pMemRT);
	}
    return true;
}

void  Direct2DRenderTarget::GetRenderBufferData(ImageData*  pData)
{
//     if (!m_pRenderBuffer)
//         return;
//     m_pRenderBuffer->GetImageData(pData);
}


void Direct2DRenderTarget::BindHDC(HDC hDC)
{
    if (NULL == hDC)
        return;

    ClearBindInfo();
    m_hBindDC = hDC;
}

void  Direct2DRenderTarget::BindHWND(HWND hWnd)
{
    if (NULL == hWnd)
        return;

    ClearBindInfo();
    m_hBindWnd = hWnd;
}

HDC  Direct2DRenderTarget::GetBindHDC()
{
    return m_hBindDC;
}


//
// It is also possible to render using GDI on a Direct2D render target by 
// calling QueryInterface on a render target for ID2D1GdiInteropRenderTarget,
// which has GetDC and ReleaseDC methods on it that can be used to retrieve
// a GDI device context. Rendering via GDI is possible only if the render 
// target was created with the D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE flag
// set. This is useful for applications that primarily render with Direct2D 
// but have an extensibility model or other legacy content that requires the
// ability to render with GDI. 
//
// 当RenderTarget被设置了剪裁区的时候，调用ID2D1GdiInteropRenderTarget::GetDC将返回：
// #define D2DERR_RENDER_TARGET_HAS_LAYER_OR_CLIPRECT MAKE_D2DHR_ERR(0x017)
// 因此现暂时决定不支持获取HDC
//
HDC   Direct2DRenderTarget::GetHDC()
{
    if (m_hBindDC)
        return m_hBindDC;

//     ID2D1GdiInteropRenderTarget*  pGdiRT = NULL;
//     m_pRenderTarget->QueryInterface(__uuidof(ID2D1GdiInteropRenderTarget), (void**)&pGdiRT);
//     if (pGdiRT)
//     {
//         HDC hDC = NULL;
//         HRESULT hr = pGdiRT->GetDC(D2D1_DC_INITIALIZE_MODE_COPY, &hDC);
//         return hDC;
//     }
    return NULL;
}
void  Direct2DRenderTarget::ReleaseHDC(HDC hDC)
{
    if (m_hBindDC)
        return;

//     ID2D1GdiInteropRenderTarget*  pGdiRT = NULL;
//     m_pRenderTarget->QueryInterface(__uuidof(ID2D1GdiInteropRenderTarget), (void**)&pGdiRT);
//     if (pGdiRT)
//     {
//         pGdiRT->ReleaseDC(NULL);
//     }
}

//
// 1. There are several kinds of render targets that can be used to render graphics in the following ways:
//  . ID2D1HwndRenderTarget objects render content to a window.
//  . ID2D1DCRenderTarget objects render to a GDI device context.
//  . Bitmap render target objects render content to an off-screen bitmap.
//  . DXGI render target objects render to a DXGI surface for use with Direct3D.
//
// 2. RenderTarget是设备依赖型的资源
//
// 3. 从Windows8开始，不再建议使用ID2D1RenderTraget进行渲染，因为与Windows Store apps不兼容
//    改为使用device context
//
// 4. Resource既能用software创建，也能用hardware创建。但在hardware上创建和删除Resource非常昂贵。
//    因此应该尽可能的重用Resource,而不是重新创建（窗口改变大小另论）
// 
//  4.1 Bitmap 
//    显卡通常有一个最小的内存分配大小。如果分配的内存小于该值，仍然会分配最小内存单位，导致多出来
//    的那部分内存被浪费，并且无法被用于做其它事情。
//    如果是需要许多小图片，一个办法是分配一张大图，然后把小图存在这张大图片当中。但在小图片之间应该
//    保留一些padding，避免在做插值操作时出现问题。
//    建议大多数图片至少为64kb，并且限制出现小于4kb的图片（32*32的32位图片）
//
//
ID2D1RenderTarget*  Direct2DRenderTarget::CreateRenderTarget()
{
    if (m_pRenderTarget)
        return m_pRenderTarget;

    HRESULT  hr = S_OK;
    if (m_hBindDC)
    {
        ID2D1DCRenderTarget*  pRT = NULL;
        hr = g_pD2DApp->m_pD2DFactory->CreateDCRenderTarget(
            &D2D1::RenderTargetProperties(),
            &pRT);

        pRT->BindDC(m_hBindDC, NULL);  // TODO: 这个值传NULL行吗。如果不行，那么还得在ResizeRenderBuffer里面继续处理
        m_pRenderTarget = static_cast<ID2D1RenderTarget*>(pRT);
    }
    else if (m_hBindWnd)
    {
        CRect rcClient;
        GetClientRect(m_hBindWnd, &rcClient);

		D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
		rtProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
		rtProps.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
		rtProps.usage = /*D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE*/D2D1_RENDER_TARGET_USAGE_NONE;

        ID2D1HwndRenderTarget*  pRT = NULL;
        hr = g_pD2DApp->m_pD2DFactory->CreateHwndRenderTarget(
            &rtProps,
            &D2D1::HwndRenderTargetProperties(m_hBindWnd, D2D1::SizeU(rcClient.Width(), rcClient.Height())),
            &pRT);

        m_pRenderTarget = static_cast<ID2D1RenderTarget*>(pRT);
    }
#if 0
    else if (m_pRenderBuffer)
    {
		//
		// 若要将 IWICBitmapLock 与 CreateSharedBitmap 方法结合使用，
		// 呈现器目标必须使用软件呈现。若要强制呈现器目标使用软件呈现，
		// 请将 D2D1_RENDER_TARGET_TYPE_SOFTWARE 设置为用于创建该呈
		// 现器目标的 D2D1_RENDER_TARGET_PROPERTIES 结构的 type 字段。
		// 若要检查现有呈现器目标是否使用软件呈现，请使用 IsSupported 
		// 方法。
		//

        
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
        rtProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
        rtProps.type = D2D1_RENDER_TARGET_TYPE_SOFTWARE/*D2D1_RENDER_TARGET_TYPE_DEFAULT*/;
        rtProps.usage = D2D1_RENDER_TARGET_USAGE_NONE;

        hr = g_pD2DApp->m_pD2DFactory->CreateWicBitmapRenderTarget(
            m_pRenderBuffer->m_pWICBitmap,
            rtProps,
            &m_pRenderTarget
            );
    }
#else
    else if (m_pSrcRenderTarget)
    {
        if (!m_pSrcRenderTarget->m_pRenderTarget)
        {
            m_pSrcRenderTarget->CreateRenderTarget();
        }

        ID2D1BitmapRenderTarget*  pMemRT = NULL;
        HRESULT hr = m_pSrcRenderTarget->m_pRenderTarget->CreateCompatibleRenderTarget(
            NULL, NULL, NULL, D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_NONE/*D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_GDI_COMPATIBLE*/,
            &pMemRT);
        m_pRenderTarget = static_cast<ID2D1RenderTarget*>(pMemRT);
    }
#endif

    if (m_pRenderTarget)
    {
        m_pRenderTarget->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

        // Set the text antialias mode to D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE explicitly. 
        // The quality of rendering grayscale text is comparable to ClearType but is much faster.
        m_pRenderTarget->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);


#ifdef _DEBUG
        // 要求一定要支持硬件加速，否则使用GDI
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
        rtProps.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
        UIASSERT(m_pRenderTarget->IsSupported(rtProps));
#endif
    }

    
    return m_pRenderTarget;
}

bool  Direct2DRenderTarget::BeginDraw()
{
    if (m_lDrawRef > 0)
        return false;
    m_lDrawRef++;

    HRESULT hr = S_OK;
    if (!m_pRenderTarget)
    {
        if (!CreateRenderTarget())
            return false;
    }

    m_pRenderTarget->BeginDraw();
    //m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::Black));
    return true;
}

// In Direct2D, all the rendering commands are enclosed between 
// a call to BeginDraw and a call to EndDraw. These calls are 
// made to a render target. You must call the BeginDraw method
// before you call rendering operations . After you call BeginDraw,
// a context typically builds up a batch of rendering commands,
// but delays processing of these commands until one of these
// statements is true:
//
//  . EndDraw occurred. When EndDraw is called, it causes any batched 
//    drawing operations to complete and returns the status of the operation.
//
//  . You make an explicit call to Flush: The Flush method causes the 
//    batch to be processed and all pending commands to be issued.
//    (MS 不建议我们调用该函数，而是由Direct2D自己进行资源管理）
//
//  . The buffer holding the rendering commands is full. If this buffer 
//    becomes full before the previous two conditions are fulfilled, the 
//    rendering commands are flushed out.
//
void  Direct2DRenderTarget::EndDraw()
{
    if (!m_pRenderTarget)
        return;

    m_lDrawRef--;
    if (m_lDrawRef < 0)
        return;

    SelectClipRgn(NULL, RGN_COPY);
	SetViewportOrgEx(0, 0, NULL);

    HRESULT hr = m_pRenderTarget->EndDraw(NULL, NULL);
    if (hr == D2DERR_RECREATE_TARGET)
    {
        // 释放设置依赖型的资源
        SAFE_RELEASE(m_pRenderTarget);
    }
    else if (hr == D2DERR_PUSH_POP_UNBALANCED)
    {
        UI_LOG_ERROR(_T("%s EndDraw Error: D2DERR_PUSH_POP_UNBALANCED"), FUNC_NAME);
    }
	else if (hr != S_OK)
	{
		UIASSERT (0);
	}
}

// 如果呈现器目标具有活动的剪辑（由 PushAxisAlignedClip 指定），则清除命令仅应用于剪辑区域内的区域
void  Direct2DRenderTarget::Clear(DWORD dwColor, RECT* prc)
{
    // 不能使用fillrect，如RGBA(0,0,0,0)由于透明度为0，导致没有效果
//     FillRect(prc, &c);

    Color c;
    c.m_col = dwColor;

    float  fR = c.r/255.0f;
    float  fG = c.g/255.0f;
    float  fB = c.b/255.0f;
    float  fAlpha = (c.a)/255.0f;

    if (prc)
    {
        UIASSERT(0 == m_lClipPushRef);  // 要求外部先调用clear，再去设置rgn，否则这里将覆盖掉原来的rgn

        m_pRenderTarget->PushAxisAlignedClip(
            D2D1::RectF(
            (float)(prc->left - m_ptOffset.x),
            (float)(prc->top  - m_ptOffset.y),
            (float)(prc->right  - m_ptOffset.x),
            (float)(prc->bottom - m_ptOffset.y)
            )
            , D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

        m_pRenderTarget->Clear(D2D1::ColorF(fR,fG,fB,fAlpha));
        m_pRenderTarget->PopAxisAlignedClip();
    }
    else
    {
        m_pRenderTarget->Clear(D2D1::ColorF(fR,fG,fB,fAlpha));
    }
}

void  Direct2DRenderTarget::FillRect(const RECT* prc, UI::Color* pColor)
{
    ID2D1SolidColorBrush*  pBrush = NULL;

    float  fR = pColor->r/255.0f;
    float  fG = pColor->g/255.0f;
    float  fB = pColor->b/255.0f;
    float  fAlpha = (pColor->a)/255.0f;
    HRESULT hr = m_pRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(fR, fG, fB, fAlpha),
        &pBrush
        );

    m_pRenderTarget->FillRectangle(
        D2D1::RectF((FLOAT)prc->left, (FLOAT)prc->top, (FLOAT)prc->right, (FLOAT)prc->bottom),
        pBrush
        );
    SAFE_RELEASE(pBrush);
}


int  Direct2DRenderTarget::SelectClipRgn(HRGN hRgn, int nMode)
{
    UIASSERT(nMode == RGN_COPY);  // 不支持其它类型

	SAFE_ARRAY_DELETE(m_pArrClipRect);
	m_nClipRectCount = 0;
    
    if (hRgn)
    {
        RGNDATA*  pData;
        DWORD  dwSize = GetRegionData(hRgn, 0, NULL);
        pData = (RGNDATA*)new byte[dwSize];
        GetRegionData(hRgn, dwSize, pData);

		m_nClipRectCount = pData->rdh.nCount;
		if (m_nClipRectCount)
			m_pArrClipRect = new RECT[pData->rdh.nCount];

        RECT*  prc = (LPRECT)pData->Buffer;
        for (unsigned int i = 0; i < pData->rdh.nCount; i++)
        {
            CopyRect(&m_pArrClipRect[i], &prc[i]);
        }

        SAFE_ARRAY_DELETE(pData);
    }

    UpdateClipRegion();
    return 0;
}
void   Direct2DRenderTarget::UpdateClipRegion()
{
    while (m_lClipPushRef)
    {
        m_pRenderTarget->PopAxisAlignedClip();
        m_lClipPushRef--;
    }

    for (int i = 0; i < m_nClipRectCount; i++)
    {
        m_pRenderTarget->PushAxisAlignedClip(
            D2D1::RectF(
            (float)(m_pArrClipRect[i].left - m_ptOffset.x),
            (float)(m_pArrClipRect[i].top  - m_ptOffset.y),
            (float)(m_pArrClipRect[i].right  - m_ptOffset.x),
            (float)(m_pArrClipRect[i].bottom - m_ptOffset.y)
            )
            , D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        m_lClipPushRef++;
    }
}

BOOL Direct2DRenderTarget::GetViewportOrgEx(LPPOINT lpPoint)
{
    if (NULL == lpPoint)
    {
        return FALSE;
    }

    D2D1_MATRIX_3X2_F  matrix;
    m_pRenderTarget->GetTransform(&matrix);
    lpPoint->x = round(matrix._31);
    lpPoint->y = round(matrix._32);

    return TRUE;
}
BOOL Direct2DRenderTarget::SetViewportOrgEx(int x, int y, LPPOINT lpPoint)
{
    if (lpPoint)
    {
        D2D1_MATRIX_3X2_F  matrix;
        m_pRenderTarget->GetTransform(&matrix);
        lpPoint->x = round(matrix._31);
        lpPoint->y = round(matrix._32);
    }

    m_ptOffset.x = x;
    m_ptOffset.y = y;

    m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation((float)x, (float)y));
    UpdateClipRegion();
    return TRUE;
}
BOOL  Direct2DRenderTarget::OffsetViewportOrgEx( int x, int y, LPPOINT lpPoint) 
{
    D2D1::Matrix3x2F  matrix;
    m_pRenderTarget->GetTransform(&matrix);

    if (lpPoint)
    {
        lpPoint->x = round(matrix._31);
        lpPoint->y = round(matrix._32);
    }

    m_ptOffset.x = round(matrix._31);
    m_ptOffset.y = round(matrix._32);

   m_pRenderTarget->SetTransform(D2D1::Matrix3x2F::Translation((float)x+matrix._31, (float)y+matrix._32));
   UpdateClipRegion();
   return TRUE;
}


void Direct2DRenderTarget::DrawBitmap(IRenderBitmap* pBitmap, DRAWBITMAPPARAM* pParam)
{
    if (NULL == pBitmap || NULL == pParam)
        return;

    if (pBitmap->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D)
        return;

    ID2D1Bitmap* pD2DBitmap = ((Direct2DRenderBitmap*)pBitmap)->GetBitmap(m_pRenderTarget);
    if (NULL == pD2DBitmap)
    {
        UI_LOG_WARN(_T("%s GetBitmap failed."), FUNC_NAME);
        return;
    }

    if (pParam->nFlag & DRAW_BITMAP_DISABLE)
    {
        UIASSERT(0);
    }

    if (pParam->nFlag & DRAW_BITMAP_BITBLT)
    {
        m_pRenderTarget->DrawBitmap(
            pD2DBitmap,
            D2D1::RectF(
            (FLOAT)pParam->xDest,
            (FLOAT)pParam->yDest,
            (FLOAT)pParam->xDest+pParam->wDest,
            (FLOAT)pParam->yDest+pParam->hDest)
            );
    }
    else if (pParam->nFlag & DRAW_BITMAP_STRETCH)
    {
       DrawBitmap(pBitmap, 
           pParam->xDest,
           pParam->yDest, 
           pParam->wDest,
           pParam->hDest,
           pParam->xSrc,
           pParam->ySrc,
           pParam->wSrc,
           pParam->hSrc,
           pParam->pRegion,
           true);
    }
    else if (pParam->nFlag & DRAW_BITMAP_STRETCH_BORDER)
    {
        DrawBitmap(pBitmap, 
            pParam->xDest,
            pParam->yDest, 
            pParam->wDest,
            pParam->hDest,
            pParam->xSrc,
            pParam->ySrc,
            pParam->wSrc,
            pParam->hSrc,
            pParam->pRegion,
            false);
    }
    else if (pParam->nFlag & DRAW_BITMAP_TILE)
    {
        UIASSERT(0);
    }
    else if (pParam->nFlag & DRAW_BITMAP_CENTER)
    {
        UIASSERT(0);
    }
    else if (pParam->nFlag & DRAW_BITMAP_ADAPT)
    {
        UIASSERT(0);
    }
}

void   Direct2DRenderTarget::BitBlt(int xDest, int yDest, int wDest, int hDest, ID2D1Bitmap* pBitmap, int xSrc, int ySrc)
{
    m_pRenderTarget->DrawBitmap(
        pBitmap,
        D2D1::RectF(
            (FLOAT)xDest,
            (FLOAT)yDest,
            (FLOAT)xDest+wDest,
            (FLOAT)yDest+hDest),
        1.0f,
        D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
        D2D1::RectF(
            (FLOAT)xSrc,
            (FLOAT)ySrc,
            (FLOAT)xSrc+wDest,
            (FLOAT)ySrc+hDest)
        );
}
void   Direct2DRenderTarget::StretchBlt(int xDest, int yDest, int wDest, int hDest, ID2D1Bitmap* pBitmap, int xSrc, int ySrc, int wSrc, int hSrc)
{
    m_pRenderTarget->DrawBitmap(
            pBitmap,
            D2D1::RectF(
                (FLOAT)xDest,
                (FLOAT)yDest,
                (FLOAT)(xDest+wDest),
                (FLOAT)(yDest+hDest)),
            1.0f, 
            D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
            D2D1::RectF(
                (FLOAT)xSrc,
                (FLOAT)ySrc,
                (FLOAT)(xSrc+wSrc),
                (FLOAT)(ySrc+hSrc))
            );
}

// 拉伸绘制
// 九宫绘制
void Direct2DRenderTarget::DrawBitmap(IRenderBitmap* pD2DBitmap, int xDest, int yDest, int wDest, 
					        int hDest, int xSrc, int ySrc, int wSrc, int hSrc,
					        C9Region* pImage9Region, bool bDrawCenter)
{
	if (NULL == pD2DBitmap)
		return;

    if (pD2DBitmap->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D)
        return;

    Direct2DRenderBitmap* pRenderBitmap = static_cast<Direct2DRenderBitmap*>(pD2DBitmap);
    ID2D1Bitmap* pBitmap = pRenderBitmap->GetBitmap(m_pRenderTarget);
    if (NULL == pBitmap)
        return;

	if (NULL == pImage9Region)
    {
        StretchBlt(xDest, yDest, wDest, hDest, pBitmap, xSrc, ySrc, wSrc, hSrc);
        return;
    }


	// 1. topleft
    BitBlt(xDest, yDest, pImage9Region->topleft, pImage9Region->top, pBitmap, xSrc, ySrc);

	// 2. top
    StretchBlt(
        xDest + pImage9Region->topleft, 
        yDest, 
        wDest - pImage9Region->topleft - pImage9Region->topright, 
        pImage9Region->top,
        pBitmap, 
        xSrc + pImage9Region->topleft, 
        ySrc, 
        wSrc - pImage9Region->topleft - pImage9Region->topright, 
        pImage9Region->top);

    // 3. topright
    BitBlt(
        xDest + wDest - pImage9Region->topright,
        yDest,
        pImage9Region->topright,
        pImage9Region->top,
        pBitmap,
        xSrc + wSrc - pImage9Region->topright,
        ySrc);

	// 4. left
    StretchBlt(xDest, 
        yDest+pImage9Region->top, 
        pImage9Region->left, 
        hDest - pImage9Region->top- pImage9Region->bottom,
        pBitmap,
        xSrc,
        ySrc + pImage9Region->top,
        pImage9Region->left,
        hSrc - pImage9Region->top - pImage9Region->bottom
        );

	// 5. center

	if (bDrawCenter)
	{
        StretchBlt(
            xDest + pImage9Region->left, 
            yDest + pImage9Region->top, 
            wDest - pImage9Region->left - pImage9Region->right,
            hDest - pImage9Region->top  - pImage9Region->bottom,
            pBitmap,
            xSrc + pImage9Region->left,
            ySrc + pImage9Region->top,
            wSrc - pImage9Region->left - pImage9Region->right,
            hSrc - pImage9Region->top  - pImage9Region->bottom
            );
	}

	// 6. right
    StretchBlt(
        xDest + wDest - pImage9Region->right, 
        yDest + pImage9Region->top, 
        pImage9Region->right,
        hDest - pImage9Region->top - pImage9Region->bottom,
        pBitmap,
        xSrc + wSrc - pImage9Region->right,
        ySrc + pImage9Region->top,
        pImage9Region->right,
        hSrc - pImage9Region->top - pImage9Region->bottom
        );

	// 7. bottomleft
    BitBlt(
        xDest,
        yDest + hDest - pImage9Region->bottom,
        pImage9Region->bottomleft, 
        pImage9Region->bottom,
        pBitmap,
        xSrc,
        ySrc + hSrc - pImage9Region->bottom
        );

	// 8. bottom
    StretchBlt(
        xDest + pImage9Region->bottomleft, 
        yDest + hDest - pImage9Region->bottom, 
        wDest - pImage9Region->bottomleft - pImage9Region->bottomright,
        pImage9Region->bottom,
        pBitmap,
        xSrc + pImage9Region->bottomleft,
        ySrc + hSrc - pImage9Region->bottom,
        wSrc - pImage9Region->bottomleft - pImage9Region->bottomright,
        pImage9Region->bottom
        );

	// 9. bottomright

    BitBlt(
        xDest + wDest - pImage9Region->bottomright,
        yDest + hDest - pImage9Region->bottom,
        pImage9Region->bottomright,
        pImage9Region->bottom,
        pBitmap,
        xSrc + wSrc - pImage9Region->bottomright,
        ySrc + hSrc - pImage9Region->bottom
        );
}

void  Direct2DRenderTarget::DrawString(IRenderFont* pFont, DRAWTEXTPARAM* pParam)
{
    if (!pFont || !pParam)
        return;

    if (NULL == pParam->prc || pParam->prc->Width() <= 0 || pParam->prc->Height() <= 0)
        return;

    if (NULL == m_pRenderTarget)
        return;

    if (pFont->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D)
    {
        UI_LOG_WARN(_T("%s font render type != GRAPHICS_RENDER_TYPE_D2D"), FUNC_NAME);
        return;
    }

    switch (pParam->nEffectFlag)
    {
    case TEXT_EFFECT_NONE:
        {
            IDWriteTextFormat* pTextFormat = ((Direct2DRenderFont*)pFont)->GetFont();
            if (NULL == pTextFormat)
            {
                UI_LOG_WARN(_T("%s NULL == pTextFormat"), FUNC_NAME);
                return;
            }

            ID2D1SolidColorBrush* pTextBrush = NULL;
            m_pRenderTarget->CreateSolidColorBrush(
                D2D1::ColorF(pParam->color),
                &pTextBrush
                );
            if (NULL == pTextBrush)
            {
                UI_LOG_WARN(_T("%s CreateSolidColorBrush failed."), FUNC_NAME);
                return;
            }

            if (pParam->nFormatFlag & DT_CENTER)
                pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
            else if (pParam->nFormatFlag & DT_RIGHT)
                pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
            else
                pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

            if (pParam->nFormatFlag & DT_VCENTER)
                pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
            else if (pParam->nFormatFlag & DT_BOTTOM)
                pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
            else
                pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);


            m_pRenderTarget->DrawText(
                pParam->szText,
                _tcslen(pParam->szText),
                pTextFormat,
                D2D1::RectF(
                (FLOAT)pParam->prc->left, 
                (FLOAT)pParam->prc->top, 
                (FLOAT)(pParam->prc->right-pParam->prc->left), 
                (FLOAT)(pParam->prc->bottom-pParam->prc->top)),
                pTextBrush
                );

            SAFE_RELEASE(pTextBrush);
        }
        break;
    default:
        break;
    }
}


void  Direct2DRenderTarget::Render2Target(IRenderTarget* pDst, Render2TargetParam* pParam)
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

	HRESULT hr = S_OK;

    if (!m_pSrcRenderTarget || !pDst)
        return;
    if (!m_pRenderTarget)
        return;

	if (pDst->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D)
		return;
	Direct2DRenderTarget*  pD2DRT = static_cast<Direct2DRenderTarget*>(pDst);

	ID2D1Bitmap*  pBitmap = NULL;
	static_cast<ID2D1BitmapRenderTarget*>(m_pRenderTarget)->GetBitmap(&pBitmap);

	// 要Sharing an IWICBitmapLock，则rendertarget必须是software的
//     IWICBitmapLock*  pLock = NULL;
// 	m_pRenderBuffer->LockFull(&pLock);
// 	hr = pD2DRT->m_pRenderTarget->CreateSharedBitmap(
// 		__uuidof(IWICBitmapLock),
// 		(void*)pLock,
// 		NULL,
// 		&pSharedBitmap
// 		);

	// 注: 1. 这里不能调用自己的m_pRenderTarget来创建，必须用目标RT
	//     2. 但这种方式是复制了一张新图片，而不是共享
// 	hr = pD2DRT->m_pRenderTarget->CreateBitmapFromWicBitmap(
// 		m_pRenderBuffer->m_pWICBitmap, &pSharedBitmap);


    if (pParam->pTransform)
    {
        D2D1::Matrix3x2F  matrixOffset = D2D1::Matrix3x2F::Translation((FLOAT)pD2DRT->m_ptOffset.x, (FLOAT)pD2DRT->m_ptOffset.y);
        D2D1::Matrix3x2F  matrixParam(
            pParam->pTransform->eM11,
            pParam->pTransform->eM12,
            pParam->pTransform->eM21,
            pParam->pTransform->eM22, 
            pParam->pTransform->eDx, 
            pParam->pTransform->eDy);
        D2D1::Matrix3x2F matrix = matrixParam*matrixOffset;
        pD2DRT->m_pRenderTarget->SetTransform(matrix);
    }
	 pD2DRT->m_pRenderTarget->DrawBitmap(
		pBitmap,
		D2D1::RectF(
		(FLOAT)xDst,
		(FLOAT)yDst,
		(FLOAT)(xDst+wDst),
		(FLOAT)(yDst+hDst)),
		opacity/255.f, 
		D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		D2D1::RectF(
		(FLOAT)xSrc,
		(FLOAT)ySrc,
		(FLOAT)(xSrc+wSrc),
		(FLOAT)(ySrc+hSrc))
		);
     if (pParam->pTransform)
     {
        pD2DRT->SetViewportOrgEx(pD2DRT->m_ptOffset.x, pD2DRT->m_ptOffset.y);
     }

//	SAFE_RELEASE(pSharedBitmap);
//	SAFE_RELEASE(pLock);
}

void  Direct2DRenderTarget::Save(const TCHAR*  szPath)
{
}

void  Direct2DRenderTarget::GradientFillH(const CRect* lprc, COLORREF colFrom, COLORREF colTo)
{
    UIASSERT(0);
//     m_pRenderTarget->CreateLinearGradientBrush(
// 
//         )
}
void  Direct2DRenderTarget::GradientFillV(const CRect* lprc, COLORREF colFrom, COLORREF colTo)
{
    UIASSERT(0);   
}
void  Direct2DRenderTarget::TileRect(const CRect* lprc, IRenderBitmap*)
{
    // D2D1BitmapBrush paints an area with a bitmap
}
}