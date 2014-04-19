#pragma once
#include "gdipluspen.h"
#include "gdiplusbitmap.h"
#include "gdiplusfont.h"

//
// 2014.4.10
// Graphics::SetClip效率非常低，在落叶Demo中，SetClip直接将CPU干满，去掉SetClip则恢复为0
// 为了解决该问题，决定只在每次需要Graphics对象时才从hdc创建一个，
// 其它时候不保留该对象，避免每次设置剪裁和偏移时，还得同步给Graphics。
// 另外由于使用Gdiplus时，大部分的绘图工作还是交给了gdi的alphablend
//
namespace UI
{
class RenderBuffer;

class GdiplusRenderTarget : public IRenderTarget
{
public:
 	GdiplusRenderTarget();
	virtual ~GdiplusRenderTarget();
    virtual void  Release();
	virtual GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() { return GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS; }

	virtual void     BindHDC(HDC hDC);
	virtual HDC      GetBindHDC();
    virtual HDC      GetHDC();
    virtual void     ReleaseHDC(HDC hDC);
    virtual bool     CreateRenderBuffer(IRenderTarget*  pSrcRT);
    virtual bool     ResizeRenderBuffer(unsigned int nWidth, unsigned int nHeight);
	virtual void     GetRenderBufferData(ImageData*  pData);
    virtual void     BindHWND(HWND hWnd) { /*UIASSERT(0);*/ /*不支持*/ }

	virtual HRGN     GetClipRgn();
	virtual int      SelectClipRgn( HRGN hRgn, int nMode = RGN_COPY );
	virtual BOOL     GetViewportOrgEx( LPPOINT lpPoint );
	virtual BOOL     SetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL ) ;
	virtual BOOL     OffsetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL );

	virtual bool     BeginDraw();
	virtual void     EndDraw();
	virtual void     Clear(DWORD dwColor, RECT* prc);
	virtual void     Save(const TCHAR* szPath);
    virtual void     Render2DC(HDC hDC, Render2TargetParam* pParam);
    virtual void     Render2Target(IRenderTarget* pDst, Render2TargetParam* pParam);

	virtual void     FillRgn( HRGN hRgn, UI::Color* pColor);
	virtual void     FillRect( const RECT* lprc, UI::Color* pColor);
	virtual void     TileRect( const CRect* lprc, IRenderBitmap* hBitmap );
	virtual void     Rectangle( const CRect* lprc, UI::Color* pColBorder, UI::Color* pColBack, int nBorder, bool bNullBack );
	virtual void     DrawFocusRect( const CRect* lprc );
	virtual void     DrawLine(int x1, int y1, int x2, int y2, IRenderPen*);
	virtual void     DrawPolyline(POINT* lppt, int nCount, IRenderPen*);
	virtual void     GradientFillH( const CRect* lprc, COLORREF colFrom, COLORREF colTo );
	virtual void     GradientFillV( const CRect* lprc, COLORREF colFrom, COLORREF colTo );
	virtual void     BitBlt( int xDest, int yDest, int wDest, int hDest, IRenderTarget* pSrcHDC, int xSrc, int ySrc, DWORD dwRop );
	virtual void     ImageList_Draw( IRenderBitmap* hBitmap, int x, int y, int col, int row, int cx, int cy );
	
    virtual void     DrawBitmap(IRenderBitmap* hBitmap, DRAWBITMAPPARAM* pParam );
    virtual void     DrawString(IRenderFont* pFont, DRAWTEXTPARAM* pParam);

    virtual IRenderPen*     CreateSolidPen(int nWidth, Color* pColor);
    virtual IRenderPen*     CreateDotPen(int nWidth, Color* pColor); 
    virtual IRenderBrush*   CreateSolidBrush(Color* pColor);

public:
	static void  DrawBitmap(Gdiplus::Graphics* pGraphics, IRenderBitmap* hBitmap, int x, int y );
	static void  DrawBitmap(Gdiplus::Graphics* pGraphics, IRenderBitmap* pBitmap, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc);
	static void  DrawBitmap(Gdiplus::Graphics* pGraphics, IRenderBitmap* hBitmap, int xDest, int yDest, int nDestWidth, 
							int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, Gdiplus::ImageAttributes* pAttr=NULL);
	static void  DrawBitmap(Gdiplus::Graphics* pGraphics, IRenderBitmap* hBitmap, int xDest, int yDest, int nDestWidth, 
							int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
                            C9Region* p9Region, bool bDrawCenter=true, Gdiplus::ImageAttributes* pAttr=NULL);		
	static void  DrawBitmapEx(HDC hBindDC, IRenderBitmap* pBitmap, DRAWBITMAPPARAM* pParam);
    static void  DrawStringEx(HDC hBindDC, IRenderFont*  pFont, DRAWTEXTPARAM* pParam);

    static void  GetStringFormatByGdiFormat(UINT, Gdiplus::StringFormat* p);
protected:
//	Gdiplus::Graphics*  m_pGraphics;
//	Gdiplus::Bitmap*    m_pGdiMemBitmap;
//	HRGN     m_hRgnMeta;   // 用于模拟实现GDI的SetMetaRgn函数

    HDC      m_hBindDC;
    RenderBuffer*  m_pRenderBuffer;
    long     m_lDrawingRef; // 标识外部调用了几次BeginDraw，解决嵌套调用出现的一些问题

#ifdef _DEBUG
	POINT  m_ptOffset;   // 用于调试时查看当前HDC偏移量
#endif
};
}