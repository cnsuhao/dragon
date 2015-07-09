#include "stdafx.h"
#include "roundrect.h"
#include "UISDK\Kernel\Src\Atl\image.h"

using namespace UI;

// TODO: 如何优化除法处理

RoundRectHandler::RoundRectHandler()
{
    lefttop = leftbottom = righttop = rightbottom = 0;

    m_hBitmap = NULL;
    m_hDC = NULL;
    m_nPitch = 0;
    m_pBits = NULL;
    m_nBitmapWidth = m_nBitmapHeigth = 0;
    m_nBitmapAvailableWidth = m_nBitmapAvailableHeight = 0;
//    m_bHandleBorder = true;
}
RoundRectHandler::~RoundRectHandler()
{
    DestroyBuffer();
    if (m_hDC)
        DeleteDC(m_hDC);
}
//
// 将TOP 、BOTTOM 这两行虚化，与圆角进行衔接
//
// pDstBits： 这一行开始的字节（0,0）或者（0, bottom）
//
// void RoundRectHandler::HandleXLine(byte* pDstBits, int xFrom, int xTo)
// {
//     int i = 0;
//     for (int x = xFrom; x < xTo; x++)
//     {
//         i = x<<2;
//         pDstBits[i++] = pDstBits[i]>>1;
//         pDstBits[i++] = pDstBits[i]>>1;
//         pDstBits[i++] = pDstBits[i]>>1;
//         pDstBits[i]   = pDstBits[i]>>1;
//     }
// }
//
// 将LEFT、RIGHT 这两列进行虚化，与圆角进行衔接
// 
// pDstBits：这一列开始的位置（0,0）或者（right, 0）
// void RoundRectHandler::HandleYLine(byte* pDstBits, int nDstPitch, int yFrom, int yTo)
// {
//     pDstBits = pDstBits + (yFrom)*nDstPitch;
//     for (int y = yFrom; y < yTo; y++)
//     {
//         pDstBits[0] = pDstBits[0]>>1;
//         pDstBits[1] = pDstBits[1]>>1;
//         pDstBits[2] = pDstBits[2]>>1;
//         pDstBits[3] = pDstBits[3]>>1;
// 
//         pDstBits += nDstPitch;
//     }
// }


void  RoundRectHandler::HandleRightTopCorner(byte* pDstBits, int nDstPitch, int xDst,
                           byte* pSrcBits, int nSrcPitch, int xSrc,
                           int nLoop)
{
    byte a = 0;  // alpha
    byte aOrigin = 0; // 原始alpha
    byte aLast = 0;   // 最终alpha
    int i = 0;  // x index

    int xLoop = 0;
    for (int y = 0; y < nLoop; y++, xLoop++)
    {
        for (int x = nLoop-1; x >= xLoop; x--)
        {
            i = (xDst+x)<<2;
            a = pSrcBits[((xSrc+x)<<2)+3];
            aOrigin = pDstBits[i+3];

            if (aOrigin)
            {
                aLast = (aOrigin*a)>>8;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i] = aLast;
            }
        }
        pDstBits += nDstPitch;
        pSrcBits += m_nPitch;
    }
}

void  RoundRectHandler::HandleRightBottomCorner(byte* pDstBits, int nDstPitch, int xDst,
                              byte* pSrcBits, int nSrcPitch, int xSrc,
                              int nLoop)
{
    byte a = 0;  // alpha
    byte aOrigin = 0; // 原始alpha
    byte aLast = 0;   // 最终alpha
    int i = 0;  // x index

#if 1
    int xLoop = nLoop-1;
    for (int y = 0; y < nLoop; y++,xLoop--)
    {
        for (int x = nLoop-1; x >= xLoop; x--)
#else
    for (int y = 0; y < nLoop; y++)
    {
        for (int x = 0; x < nLoop; x++)
#endif
        {
            i = (xDst+x)<<2;
            a = pSrcBits[((xSrc+x)<<2)+3];
            aOrigin = pDstBits[i+3];

            if (aOrigin)
            {
                aLast = (aOrigin*a)>>8;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i] = aLast;
            }
        }
        pDstBits += nDstPitch;
        pSrcBits += m_nPitch;
    }
}

void  RoundRectHandler::HandleLeftBottomCorner(byte* pDstBits, int nDstPitch, int xDst,
                             byte* pSrcBits, int nSrcPitch, int xSrc,
                             int nLoop)
{
    byte a = 0;  // alpha
    byte aOrigin = 0; // 原始alpha
    byte aLast = 0;   // 最终alpha
    int i = 0;  // x index

#if 1
    int xLoop = 0;
    for (int y = 0; y < nLoop; y++, xLoop++)
    {
        for (int x = 0; x <= xLoop; x++)
#else
    for (int y = 0; y < nLoop; y++)
    {
        for (int x = 0; x < nLoop; x++)
#endif
        {
            i = (xDst+x)<<2;
            a = pSrcBits[((xSrc+x)<<2)+3];
            aOrigin = pDstBits[i+3];

            if (aOrigin)
            {
                aLast = (aOrigin*a)>>8;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i] = aLast;
            }
        }
        pDstBits += nDstPitch;
        pSrcBits += m_nPitch;
    }
}

void  RoundRectHandler::HandleLeftTopCorner(byte* pDstBits, int nDstPitch, int xDst,
                          byte* pSrcBits, int nSrcPitch, int xSrc,
                          int nLoop)
{
    byte a = 0;  // alpha
    byte aOrigin = 0; // 原始alpha 
    byte aLast = 0;   // 最终alpha
    int i = 0;  // x index

    int xLoop = nLoop;
    for (int y = 0; y < nLoop; y++, xLoop--)
    {
        for (int x = 0; x < xLoop; x++)
        {
            i = (xDst+x)<<2;
            a = pSrcBits[((xSrc+x)<<2)+3];
            aOrigin = pDstBits[i+3];

            if (aOrigin)
            {
                aLast = (aOrigin*a)>>8;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i++] = pDstBits[i]*aLast/aOrigin;
                pDstBits[i] = aLast;
            }
        }
        pDstBits += nDstPitch;
        pSrcBits += m_nPitch;
    }
}


// 注：hBitmap为带预乘结果的位图数据(Gdiplus的结果是带预乘的)
bool  RoundRectHandler::Apply2Bitmap(HBITMAP hBitmap, bool bTopDown, RECT* prc)
{
    if (!hBitmap || !m_hBitmap)
        return false;

    DIBSECTION  dib = {0};
    if (sizeof(dib) != GetObject(hBitmap, sizeof(dib), &dib))
        return false;

    BITMAP& bm = dib.dsBm;

    UINT  bmWidth = 0;
    UINT  bmHeight = 0;
    if (prc)
    {
        // 没处理这种情况
        assert (prc->left == 0 && prc->top == 0);

        bmWidth = prc->right - prc->left;
        bmHeight = prc->bottom - prc->top;
    }
    else
    {
        bmWidth = bm.bmWidth;
        bmHeight = bm.bmHeight;
    }

    if (bmWidth < GetMinWidth() || 
        bmHeight < GetMinHeight())
        return false;

    byte*  pDst = (byte*)bm.bmBits;
    int    nDstPitch = bm.bmWidthBytes;
    if (!bTopDown)
    {
        pDst = pDst + (bmHeight-1)*nDstPitch;
        nDstPitch = -nDstPitch;
    }
    byte*  pDstBits = pDst;
    byte*  pSrcBits = m_pBits;

    if (lefttop)
    {
        HandleLeftTopCorner(pDstBits, nDstPitch, 0, pSrcBits, m_nPitch, 0, lefttop);
    }

    if (leftbottom)
    {
        int yDst = bmHeight-leftbottom;
        int ySrc = m_nBitmapAvailableHeight-leftbottom; 

        pSrcBits = m_pBits + (m_nPitch)*ySrc;
        pDstBits = (pDst) + (nDstPitch)*yDst;

        HandleLeftBottomCorner(pDstBits, nDstPitch, 0, pSrcBits, m_nPitch, 0, leftbottom);
    }

    if (righttop)
    {
        pSrcBits = m_pBits;
        pDstBits = pDst;

        int xDst = bmWidth - righttop;
        int xSrc = m_nBitmapAvailableWidth - righttop;

        HandleRightTopCorner(pDstBits, nDstPitch, xDst, pSrcBits, m_nPitch, xSrc, righttop);
    }

    if (rightbottom)
    {
        int yDst = bmHeight-rightbottom;
        int ySrc = m_nBitmapAvailableHeight-rightbottom; 

        pSrcBits = m_pBits + (m_nPitch)*ySrc;
        pDstBits = (pDst) + (nDstPitch)*yDst;

        int xDst = bmWidth - rightbottom;
        int xSrc = m_nBitmapAvailableWidth - rightbottom;

        HandleRightBottomCorner(pDstBits, nDstPitch, xDst, pSrcBits, m_nPitch, xSrc, rightbottom);
    }

//     if (m_bHandleBorder)
//     {
//         // 如果圆角为0，则按顺时针方向从下一个像素开始处理。第一个像素留给后面的人处理
//         pDstBits = pDst;
//         HandleXLine(pDstBits, lefttop==0?1:lefttop, bmWidth-righttop);
//         pDstBits = pDst + (bmHeight-1)*nDstPitch;
//         HandleXLine(pDstBits, leftbottom, rightbottom==0?bmWidth-1:bmWidth-rightbottom);
// 
//         pDstBits = pDst;
//         HandleYLine(pDstBits, nDstPitch, lefttop, leftbottom==0?bmHeight-1:bmHeight-leftbottom);
//         pDstBits = pDst + (bmWidth-1)*4;
//         HandleYLine(pDstBits, nDstPitch, righttop==0?1:righttop, bmHeight-rightbottom);
//     } 

#ifdef _DEBUG
//     static bool bDebugModexxxxxxxxxxxx = false;
//     if (bDebugModexxxxxxxxxxxx)
//     {
//         CImage image;
//         image.Attach(hBitmap);
//         image.Save(L"C:\\aaa.png", ImageFormatPNG);
//         image.Detach();
//     }
#endif
    return true;
}

void  RoundRectHandler::DestroyBuffer()
{
    if (m_hBitmap)
    {
        DeleteObject(m_hBitmap);
        m_hBitmap = NULL;

        m_nBitmapWidth = 0;
        m_nBitmapHeigth = 0;
        m_nBitmapAvailableWidth = 0;
        m_nBitmapAvailableHeight = 0;
        m_nPitch = 0;
        m_pBits = NULL;
    }
}

void  RoundRectHandler::RequestBuffer(UINT nWidth, UINT nHeight)
{
    if (nWidth > m_nBitmapWidth || nHeight > m_nBitmapHeigth)
    {
        DestroyBuffer();
    }

    if (NULL == m_hDC)
    {
        m_hDC = CreateCompatibleDC(NULL);
    }

    if (NULL == m_hBitmap)
    {
        m_nBitmapWidth = nWidth;
        m_nBitmapHeigth = nHeight;

        Image image;
        image.Create(m_nBitmapWidth, -(int)m_nBitmapHeigth, 32, Image::createAlphaChannel);
        m_nBitmapAvailableWidth = m_nBitmapWidth;
        m_nBitmapAvailableHeight = m_nBitmapHeigth;

        m_pBits = (byte*)image.GetBits();
        m_nPitch = image.GetPitch();
        m_hBitmap = image.Detach();

        SelectObject(m_hDC, m_hBitmap);
    }
    else
    {
        RECT  rc = {0, 0, nWidth, nHeight};
        FillRect(m_hDC, &rc, (HBRUSH)GetStockObject(BLACK_BRUSH));

        m_nBitmapAvailableWidth = nWidth;
        m_nBitmapAvailableHeight = nHeight;
    }
}

void  RoundRectHandler::CreateMaskBitmap()
{
    int nMinWidth = GetMinWidth();
    int nMinHeight = GetMinHeight();
    if (nMinHeight == 0 || nMinHeight == 0)
        return;

    RequestBuffer(nMinWidth, nMinHeight);

    Gdiplus::GraphicsPath path;
    CreatePath(&path);

    Gdiplus::Graphics g(m_hDC);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
    Gdiplus::SolidBrush brush((Gdiplus::ARGB)Gdiplus::Color::White);
    g.FillPath(&brush, &path);

//     Pen pen(Color::White);
//     g.DrawPath(&pen, &path);

#ifdef _DEBUG
//     static bool bDebugModexxxxxxxxxxxx = false;
//     if (bDebugModexxxxxxxxxxxx)
//     {
//         CImage image;
//         image.Attach(m_hBitmap);
//         image.Save(L"C:\\aaamask.png", ImageFormatPNG);
//         image.Detach();
//     }
#endif
}

//  圆形
inline bool  IsCircle(int nRadiusLT, int nRadiusRT, int nRadiusLB, int nRadiusRB)
{
    if (nRadiusLT == nRadiusLB && 
        nRadiusLT == nRadiusRT &&
        nRadiusRT == nRadiusRB && 
        nRadiusLT != 0)
        return true;

    return false;
}

void  RoundRectHandler::CreatePath(Gdiplus::GraphicsPath* pPath)
{
    if (IsCircle(lefttop, righttop, rightbottom, leftbottom))
    {
        pPath->AddEllipse(0, 0, lefttop*2, lefttop*2);
        return;  
    }

    int width = GetMinWidth();
    int height = GetMinHeight();

    RECT rc = {0, 0, width, height};
    CreateRoundRectPath3(pPath, &rc, lefttop, righttop, leftbottom, rightbottom);
} 

UINT  RoundRectHandler::GetMinWidth()
{
    return max((lefttop + righttop), (leftbottom+rightbottom));
}
UINT  RoundRectHandler::GetMinHeight()
{
    return max((lefttop + leftbottom), (righttop+rightbottom));
}
void RoundRectHandler::Set(int n)
{
    if (lefttop == n && leftbottom == n &&
        righttop == n && rightbottom == n)
    {
        return;
    }

    lefttop = leftbottom = righttop = rightbottom = n;
    CreateMaskBitmap();
}

void  RoundRectHandler::Set(REGION4* r)
{
    this->Set(r->left,
        r->top, 
        r->right,
        r->bottom);
}

void RoundRectHandler::Set(int nLeftTop, int nRightTop, int nLeftBottom, int nRightBottom)
{
    if (lefttop == nLeftTop && leftbottom == nLeftBottom &&
        righttop == righttop && rightbottom == nRightBottom)
    {
        return;
    }

    lefttop = nLeftTop;
    leftbottom = nLeftBottom;
    righttop = nRightTop;
    rightbottom = nRightBottom;
    CreateMaskBitmap();
}

namespace UI
{
RoundRectHandler  g_CommonRoundRectHandler;
RoundRectHandler  g_Circle24_24RoundRectHandler;


// 构造一个 左半圆 +　右半圆 的圆角矩形区域
//
// 需要配合使用：
// g.SetSmoothingMode(SmoothingModeHighQuality);
// g.SetPixelOffsetMode(PixelOffsetModeHighQuality);
//
bool  CreateRoundRectPath2(Gdiplus::GraphicsPath* pPath, RECT* prc)
{
    if (!pPath || !prc)
        return false;

    // 16px的radius，画出31px的圆角区域，再经过抗锯齿变成32
    int width = prc->right - prc->left;
    int height = prc->bottom - prc->top;
    int nRadius = height >> 1;
    int nRadius2 = nRadius*2;

    // [注]如果不开启抗锯齿，这里应该用269(不是270)才能得到一个上下对称的形状
    pPath->AddArc(prc->left + width - nRadius2, prc->top, nRadius2, nRadius2, 270/*-1*/, 180);   
    pPath->AddArc(prc->left, prc->top, nRadius2, nRadius2, 90, 180);       

    // 将两个半圆连起来
    pPath->CloseFigure();    
    return true;
}

// 构造一个圆角矩形，四个圆角的半径为nRadius
bool  CreateRoundRectPath(Gdiplus::GraphicsPath* pPath, RECT* prc, int nRadius)
{
    if (!pPath || !prc)
        return false;

    int nRadius2 = nRadius*2;

    // 右上
    pPath->AddArc(prc->right - nRadius2, prc->top, nRadius2, nRadius2, 270, 90 );   
    // 右下
    pPath->AddArc(prc->right - nRadius2, prc->bottom - nRadius2, nRadius2, nRadius2, 0, 90 );  
    // 左下
    pPath->AddArc(prc->left, prc->bottom - nRadius2, nRadius2, nRadius2, 90, 90 );        
    // 左上
    pPath->AddArc(prc->left, prc->top, nRadius2, nRadius2, 180, 90);       

    pPath->CloseFigure();    
    return true;
}

bool  CreateRoundRectPath4(Gdiplus::GraphicsPath* pPath, RECT* prc, REGION4* pRadius)
{
    return CreateRoundRectPath3(pPath, prc, 
        pRadius->left, pRadius->right, pRadius->right, pRadius->bottom);
}

void  PathAddPoint(Gdiplus::GraphicsPath* pPath, int x, int y)
{
    Gdiplus::Point p;
    p.X = x;
    p.Y = y;
    pPath->AddLines(&p, 1);
}

bool  CreateRoundRectPath3(Gdiplus::GraphicsPath* pPath, RECT* prc, 
    int nRadiusLT, int nRadiusRT, int nRadiusRB, int nRadiusLB)
{
    // 注: arc使用直径
    int lt = nRadiusLT*2;
    int rt = nRadiusRT*2;
    int lb = nRadiusLB*2;
    int rb = nRadiusRB*2;

    // 右上
    if (rt)
        pPath->AddArc(prc->right - rt, prc->top, rt, rt, 270, 90 );   
    else
        PathAddPoint(pPath, prc->right, prc->top);

    // 右下
    if (rb)
        pPath->AddArc(prc->right - rb, prc->bottom - rb, rb, rb, 0, 90 );   
    else
        PathAddPoint(pPath, prc->right, prc->bottom);

    // 左下
    if (lb)
        pPath->AddArc(prc->left, prc->bottom - lb, lb, lb, 90, 90 );    
    else
        PathAddPoint(pPath, prc->left, prc->bottom);

    // 左上
    if (lt)
        pPath->AddArc(prc->left, prc->top, lt, lt, 180, 90 );            
    else
        PathAddPoint(pPath, prc->left, prc->top);

    pPath->CloseFigure(); 
    return true;
}

}