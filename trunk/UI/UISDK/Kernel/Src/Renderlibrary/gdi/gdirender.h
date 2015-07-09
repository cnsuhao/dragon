#ifndef GDIRENDER_H_231FA1F6_4F95_48f7_AAAA_574FF8D57425
#define GDIRENDER_H_231FA1F6_4F95_48f7_AAAA_574FF8D57425

#include "gdibitmap.h"
#include "gdifont.h"
#include "gdipen.h"
#include "gdibrush.h"

namespace UI
{
class RenderBuffer;

class GdiRenderTarget : public IRenderTarget
{
public:
 	GdiRenderTarget(bool bNeedAlphaChannel);
	virtual ~GdiRenderTarget();
    virtual void Release();
	virtual GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() { return GRAPHICS_RENDER_LIBRARY_TYPE_GDI; }
    virtual bool  IsRenderAlphaChannel() { return m_bNeedAlphaChannel; }

    virtual HDC      GetHDC();
    virtual void     ReleaseHDC(HDC hDC);
	virtual void     BindHDC(HDC hDC);
	virtual HDC      GetBindHDC();
    virtual bool     CreateRenderBuffer(IRenderTarget*  pSrcRT);
    virtual bool     ResizeRenderBuffer(unsigned int nWidth, unsigned int nHeight);
	virtual void     GetRenderBufferData(ImageData*  pData);
    virtual void     BindHWND(HWND hWnd) { /*UIASSERT(0);*/ /*不支持*/ }

	virtual HRGN     GetClipRgn();
	virtual int      SelectClipRgn( HRGN hRgn, int nMode = RGN_COPY );
	virtual int      SelectClipRect( RECT* prc, uint nrcCount, int nMode = RGN_COPY );
	virtual BOOL     GetViewportOrgEx( LPPOINT lpPoint );
	virtual BOOL     SetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL ) ;
	virtual BOOL     OffsetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL );

	virtual bool     BeginDraw();
	virtual void     EndDraw( );
	virtual void     Clear(RECT* prc);
	virtual void     Save(const TCHAR*  szPath );
    virtual void     Render2DC(HDC hDC, Render2TargetParam* pParam);
    virtual void     Render2Target(IRenderTarget* pDst, Render2TargetParam* pParam);

	virtual void     FillRgn(HRGN, UI::Color* pColor);
	virtual void     DrawRect(const RECT* lprc, UI::Color* pColor);
	virtual void     TileRect(const CRect* lprc, IRenderBitmap* hBitmap);
	virtual void     Rectangle(const CRect* lprc, UI::Color* pColBorder, UI::Color* pColBack, int nBorder, bool bNullBack);
	virtual void     DrawFocusRect( const CRect* lprc );
	virtual void     DrawLine(int x1, int y1, int x2, int y2, IRenderPen*);
	virtual void     DrawPolyline(POINT* lppt, int nCount, IRenderPen*);
	virtual void     GradientFillH(const CRect* lprc, COLORREF colFrom, COLORREF colTo);
	virtual void     GradientFillV(const CRect* lprc, COLORREF colFrom, COLORREF colTo);
	virtual void     BitBlt(int xDest, int yDest, int wDest, int hDest, IRenderTarget* pSrcHDC, int xSrc, int ySrc, DWORD dwRop);
	virtual void     ImageList_Draw(IRenderBitmap* hBitmap, int x, int y, int col, int row, int cx, int cy );
	virtual void     DrawBitmap(IRenderBitmap* hBitmap, DRAWBITMAPPARAM* pParam);
//	virtual void     DrawRotateBitmap(IRenderBitmap* pBitmap, int nDegree, DRAWBITMAPPARAM* pParam);
    virtual void     DrawString(IRenderFont* pFont, DRAWTEXTPARAM* pParam);
	static  void     DrawBitmapEx(HDC hDC, IRenderBitmap* hBitmap, DRAWBITMAPPARAM* pParam);

	virtual IRenderPen*     CreateSolidPen(int nWidth, Color* pColor);
	virtual IRenderPen*     CreateDotPen(int nWidth, Color* pColor);
	virtual IRenderBrush*   CreateSolidBrush(Color* pColor);

    virtual void  Upload2Gpu(IGpuLayerTexture* p, LPRECT prcArray, int nCount);

protected:
	void     DrawBitmap(IRenderBitmap* hBitmap, int x, int y);
	void     DrawBitmap(IRenderBitmap* pBitmap, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc);
	void     DrawBitmap(IRenderBitmap* hBitmap, int xDest, int yDest, int nDestWidth, 
						int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight );
	void     DrawBitmap(IRenderBitmap* hBitmap, int xDest, int yDest, int nDestWidth, 
						int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
						C9Region* p9Region );

    void  FixAlpha0To255(HDC hDC, LPCRECT lpRect);
    void  FillAlpha255(HDC hDC, LPCRECT lpRect);
	void  FillAlphaValue(HDC hDC, LPCRECT lpRect, WPARAM);
    void  InverseAlpha(HDC hDC, LPCRECT lpRect);
    void  _FixAlpha(HDC hDC, LPCRECT lpRect, Util::FixAlphaMode e, WPARAM wParam);


protected:
    HDC    m_hBindDC;
    RenderBuffer*  m_pRenderBuffer;
    bool   m_bNeedAlphaChannel;  // 需要渲染alpha通道

	long   m_lDrawingRef; // 标识外部调用了几次BeginDraw，解决嵌套调用出现的一些问题

#ifdef _DEBUG
    POINT  m_ptOffset;   // 用于调试时查看当前HDC偏移量
#endif
};

}

#endif  // GDIRENDER_H_231FA1F6_4F95_48f7_AAAA_574FF8D57425