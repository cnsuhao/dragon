#pragma once

/*

// 构造一个圆形的头像
void  CreateCirclePortrait(__in Gdiplus::Bitmap* pBitmap, int nPortraitSize, __out CImage& image)
{
    image.Destroy();
    if (!pBitmap)
        return;

    image.Create(nPortraitSize, -nPortraitSize, 32, 
            CImage::createAlphaChannel);

    HDC hDC = image.GetDC();
    {
        Graphics g(hDC);
        g.SetInterpolationMode(InterpolationModeHighQualityBicubic);
        g.SetPixelOffsetMode(PixelOffsetModeHalf);   // <-- 必须加上这个，否则画出来的图片边缘不正确

        RectF dst(0, 0, (REAL)nPortraitSize, (REAL)nPortraitSize);
        g.DrawImage(pBitmap, dst, 0, 0,
            (REAL)pBitmap->GetWidth(),
            (REAL)pBitmap->GetHeight(), UnitPixel);
    }
    image.ReleaseDC();

    g_Circle24_24RoundRectHandler.Set(nPortraitSize/2);
    g_Circle24_24RoundRectHandler.Apply2Bitmap(image, true, NULL);
}
*/

namespace UI
{

class RoundRectHandler
{
public:
    RoundRectHandler();
    ~RoundRectHandler();

    void  Set(int n);
    void  Set(REGION4* r);
    void  Set(int nLeftTop, int nRightTop, int nLeftBottom, int nRightBottom);

    bool  Apply2Bitmap(HBITMAP hBitmap, bool bTopDown, RECT* prc);
    
protected:
    UINT  GetMinWidth();
    UINT  GetMinHeight();
    void  CreatePath(Gdiplus::GraphicsPath* pPath);
    void  CreateMaskBitmap();
    void  RequestBuffer(UINT nWidth, UINT nHeight);
    void  DestroyBuffer();

    // 注：处理四个圆角时，其实只需要处理一半即可（正方形的一半，一个三角开）
    //     因此在循环过程中，通过调整xLoop来实现只处理一半的区域
    void  HandleLeftTopCorner(byte* pDstBits, int nDstPitch, int xDst,
            byte* pSrcBits, int nSrcPitch, int xSrc,
            int nLoop);
    void  HandleLeftBottomCorner(byte* pDstBits, int nDstPitch, int xDst,
            byte* pSrcBits, int nSrcPitch, int xSrc,
            int nLoop);
    void  HandleRightTopCorner(byte* pDstBits, int nDstPitch, int xDst,
            byte* pSrcBits, int nSrcPitch, int xSrc,
            int nLoop);
    void  HandleRightBottomCorner(byte* pDstBits, int nDstPitch, int xDst,
            byte* pSrcBits, int nSrcPitch, int xSrc,
            int nLoop);

//     void HandleXLine(byte* pDstBits, int xFrom, int xTo);
//     void HandleYLine(byte* pDstBits, int nDstPitch, int yFrom, int yTo);

private:
    // 四个圆角的半径
    int  lefttop;
    int  righttop;
    int  leftbottom;
    int  rightbottom;

//    bool  m_bHandleBorder;  // 是否处理四周线条

    HBITMAP  m_hBitmap;
    UINT     m_nBitmapWidth;
    UINT     m_nBitmapHeigth;
    UINT     m_nBitmapAvailableWidth;   // 由于图片共享，可能只使用了m_hBitmap中左上角的一部分
    UINT     m_nBitmapAvailableHeight;
    UINT     m_nPitch;
    byte*    m_pBits;

    HDC      m_hDC;
};

bool  CreateRoundRectPath2(Gdiplus::GraphicsPath* pPath, RECT* prc);
bool  CreateRoundRectPath(Gdiplus::GraphicsPath* pPath, RECT* prc, int nRadius);
bool  CreateRoundRectPath3(Gdiplus::GraphicsPath* pPath, RECT* prc, 
                           int nRadiusLT, int nRadiusRT, int nRadiusRB, int nRadiusLB);
bool  CreateRoundRectPath4(Gdiplus::GraphicsPath* pPath, RECT* prc, REGION4* pRadius);

extern RoundRectHandler  g_CommonRoundRectHandler;       // 普通圆角，例如panel圆角
extern RoundRectHandler  g_Circle24_24RoundRectHandler;  // 圆形头像
}