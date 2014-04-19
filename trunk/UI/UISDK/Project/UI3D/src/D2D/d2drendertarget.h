#pragma once
#include "UISDK\Kernel\Inc\Interface\renderlibrary.h"


// Direct2DRenderTarget只考虑支持硬件加速方式的渲染，软件方式的渲染直接使用gdi/gdiplus
// 也不支持需要将数据拷贝到内存的操作，如获取缓存pBits
namespace UI
{
class Direct2DRenderBuffer;

class Direct2DRenderTarget : public IRenderTarget
{
public:
    Direct2DRenderTarget();
    virtual ~Direct2DRenderTarget();
	virtual GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() { return GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D; }
    virtual void     Release();

    virtual void     BindHDC(HDC hDC);
    virtual HDC      GetBindHDC();
    virtual bool     CreateRenderBuffer(IRenderTarget*  pSrcRT);
    virtual bool     ResizeRenderBuffer(unsigned int nWidth, unsigned int nHeight);
    virtual void     GetRenderBufferData(ImageData*  pData);
    virtual HDC      GetHDC();
    virtual void     ReleaseHDC(HDC hDC);
    virtual void     BindHWND(HWND hWnd);

    virtual bool     BeginDraw();
    virtual void     EndDraw();
    virtual void     Clear(DWORD dwColor, RECT* prc);
    virtual void     Save(const TCHAR*  szPath);
    virtual void     Render2DC(HDC hDC, Render2TargetParam* pParam)  { }
    virtual void     Render2Target(IRenderTarget* pDst, Render2TargetParam* pParam);

    virtual HRGN     GetClipRgn()  {return NULL; }
    virtual int      SelectClipRgn( HRGN hRgn, int nMode = RGN_COPY);
    virtual BOOL     GetViewportOrgEx( LPPOINT lpPoint );
    virtual BOOL     SetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL);
    virtual BOOL     OffsetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL);

    virtual IRenderPen*    CreateSolidPen(int nWidth, Color* pColor)  {return NULL; }
    virtual IRenderPen*    CreateDotPen(int nWidth, Color* pColor)  { return NULL;} 
    virtual IRenderBrush*  CreateSolidBrush(Color* pColor)  {return NULL; }

    virtual void     FillRgn(HRGN hRgn, UI::Color* pColor)  { }
    virtual void     FillRect(const RECT* lprc, UI::Color* pColor);
    virtual void     TileRect(const CRect* lprc, IRenderBitmap*);
    virtual void     Rectangle(const CRect* lprc, UI::Color* pColBorder, UI::Color* pColBack, int nBorder, bool bNullBack)  { }
    virtual void     DrawFocusRect(const CRect* lprc)  { }
    virtual void     DrawLine(int x1, int y1, int x2, int y2, IRenderPen*)  { }
    virtual void     DrawPolyline(POINT* lppt, int nCount, IRenderPen*)  { }
    virtual void     GradientFillH(const CRect* lprc, COLORREF colFrom, COLORREF colTo);
    virtual void     GradientFillV(const CRect* lprc, COLORREF colFrom, COLORREF colTo);
    virtual void     BitBlt(int xDest, int yDest, int wDest, int hDest, IRenderTarget* pSrcHDC, int xSrc, int ySrc, DWORD dwRop )  { }
    virtual void     ImageList_Draw(IRenderBitmap* , int x, int y, int col, int row, int cx, int cy)  { }
    virtual void     DrawBitmap(IRenderBitmap* , DRAWBITMAPPARAM* pParam);
    virtual void     DrawString(IRenderFont* pFont, DRAWTEXTPARAM* pParam);

public:
    void   BitBlt(int xDest, int yDest, int wDest, int hDest, ID2D1Bitmap* pBitmap, int xSrc, int ySrc);
    void   StretchBlt(int xDest, int yDest, int wDest, int hDest, ID2D1Bitmap* pBitmap, int xSrc, int ySrc, int wSrc, int hSrc);

protected:
    void   ClearBindInfo();
    ID2D1RenderTarget*  CreateRenderTarget();
    void   ReleaseDeviceDependentRes();
    void   ReleaseDeviceIndependRes();

    void   UpdateClipRegion();


    void   DrawBitmap(IRenderBitmap* hBitmap, int xDest, int yDest, int nDestWidth, 
                                    int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
                                    C9Region* pImage9Region, bool bDrawCenter);

public:
    ID2D1RenderTarget*      m_pRenderTarget;
protected:    
    Direct2DRenderTarget*   m_pSrcRenderTarget;
    HDC    m_hBindDC;
    HWND   m_hBindWnd;

    long   m_lDrawRef;       // 调用BeginDraw的次数。现在只支持一次
    long   m_lClipPushRef;   // 调用PushAxisAlignedClip的次数，用于pop还原
    int    m_nClipRectCount;
	RECT*  m_pArrClipRect;
    POINT  m_ptOffset;
};

}