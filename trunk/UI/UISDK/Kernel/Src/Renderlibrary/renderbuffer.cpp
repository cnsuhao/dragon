#include "stdafx.h"
#include "renderbuffer.h"
#include "UISDK\Kernel\Src\Atl\image.h"

namespace UI
{
RenderBuffer::RenderBuffer()
{
    m_eGraphicsType= GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
    m_hDC = NULL;
    m_nWidth = 0;
    m_nHeight = 0;
    m_hBitmap = NULL;
}

RenderBuffer::~RenderBuffer()
{
	Destroy();
}

void  RenderBuffer::Destroy()
{
	SAFE_DELETE_GDIOBJECT(m_hBitmap);
    DeleteDC(m_hDC);
    m_hDC = NULL;
    m_nHeight = m_nWidth = 0;
}

void  RenderBuffer::Clear(DWORD dwColor, RECT* prc)
{
    if (!m_hBitmap)
        return;

    HBRUSH hBrush = CreateSolidBrush(dwColor);
    if (prc)
    {
        ::FillRect(m_hDC, prc, hBrush);
    }
    else
    {
        RECT rc = {0, 0, m_nWidth, m_nHeight};
        ::FillRect(m_hDC, &rc, hBrush);
    }
    
    SAFE_DELETE_GDIOBJECT(hBrush);
}

void   RenderBuffer::Resize(unsigned int width, unsigned int height)
{
    if (width == 0 || height == 0)
    {
        Destroy();
        return;
    }
    if (width == m_nWidth && height == m_nHeight)
    {
        return;
    }
    if (m_hBitmap)
    {
        SAFE_DELETE_GDIOBJECT(m_hBitmap);
    }

    Image image;
	image.Create(width, -(int)height, 32, Image::createAlphaChannel);
    m_hBitmap = image.Detach();

    if (!m_hDC)
    {
        m_hDC = CreateCompatibleDC(NULL);
        SetBkMode(m_hDC, TRANSPARENT);
        SetStretchBltMode(m_hDC, HALFTONE);
    }
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(m_hDC, m_hBitmap);

    m_nWidth = width;
    m_nHeight = height;
}

// void  RenderBuffer::Draw(HDC hDC)
// {
//     if (m_bAlphaBlend)
//     {
//         if (255 == m_nOpcity)
//         {
//             m_image.Draw(hDC, 0, 0);
//         }
//         else
//         {
//             m_image.Draw(hDC, 0, 0, m_nOpcity);
//         }
//     }
//     else
//     {
//         m_image.BitBlt(hDC, 0, 0);
//     }
// }

void  RenderBuffer::Dump()
{
    if (m_hBitmap)
    {
        Image image;
        image.Attach(m_hBitmap, Image::DIBOR_TOPDOWN);
        image.Save(L"C:\\aaa.bmp", Gdiplus::ImageFormatBMP);
        image.Save(L"C:\\aaa.png", Gdiplus::ImageFormatPNG);
        image.Detach();
    }
}

void  RenderBuffer::GetImageData(ImageData*  pData)
{
	BITMAP  bm;
	::GetObject(m_hBitmap, sizeof(bm), &bm);

	pData->m_nbpp = bm.bmBitsPixel;
	pData->m_nWidth = bm.bmWidth;
	pData->m_nHeight = bm.bmHeight;
	pData->m_nStride = bm.bmWidthBytes;
	pData->m_pScan0 = (BYTE*)bm.bmBits;
	pData->m_ptr = (BYTE*)bm.bmBits;
}

}