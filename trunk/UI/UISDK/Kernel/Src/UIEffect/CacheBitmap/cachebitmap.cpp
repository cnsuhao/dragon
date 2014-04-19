#include "stdafx.h"
#include "cachebitmap.h"
#include "UISDK\Kernel\Src\Atl\image.h"
namespace UI
{

CacheBitmap::CacheBitmap()
{
}
CacheBitmap::~CacheBitmap()
{
    Destroy();
}

void  CacheBitmap::Destroy()
{
    if (!m_buffer.IsNull())
    {
        m_buffer.Destroy();
    }
}

HBITMAP  CacheBitmap::Create(int nWidth, int nHeight)
{
    Image* p = CreateEx(nWidth, nHeight);
    if (p)
        return p->GetHBITMAP();

    return NULL;
}

Image*  CacheBitmap::CreateEx(int nWidthReq, int nHeightReq)
{
    int nWidth = 0; 
    int nHeight = 0;
    AdjustWH(nWidthReq, nHeightReq, nWidth, nHeight);

    if (!m_buffer.IsNull() && 
        (m_buffer.GetHeight() < nHeight || m_buffer.GetWidth() < nWidth))
    {
        Destroy();
    }

    if (m_buffer.IsNull())
    {
        m_buffer.Create(nWidth, -nHeight, 32, Image::createAlphaChannel);  // ������ʾ����һ��top-down DIB,����������λͼ�ܹ�ֱ�Ӷ�Ӧ�����ڼ���
    }
    else
    {
        HDC hDC = m_buffer.GetDC();
        RECT rc = {0, 0, nWidthReq, nHeightReq};
        ::FillRect(hDC, &rc, (HBRUSH)::GetStockObject(BLACK_BRUSH));
        m_buffer.ReleaseDC();
    }

    return &m_buffer;
}

void  CacheBitmap::Clear(DWORD dwColor, RECT* prc)
{
    if (m_buffer.IsNull() || NULL == prc)
        return;

    RECT rcDst;
    RECT rcTemp = { 0, 0, m_buffer.GetWidth(), m_buffer.GetHeight() };
    if (!IntersectRect(&rcDst, &rcTemp, prc))
        return;

    byte* pBits = (byte*)m_buffer.GetBits();

    pBits = (pBits + (rcDst.top * m_buffer.GetPitch() + prc->left*4));
    int nSize = (rcDst.right-rcDst.left)*4;
    for (int y = rcDst.top; y < rcDst.bottom; y++)
    {
        int* p = (int*)pBits;
        for (int x = rcDst.left; x < rcDst.right; x++, p++)
        {
            *p = dwColor;
        }
        pBits += m_buffer.GetPitch();
    }
}
// 
// void  CacheBitmap::Transparent(DWORD dwTransparent, RECT* prc)
// {
//     if (NULL == m_hCacheBitmap || NULL == prc)
//         return;
// 
//     RECT rcDst;
//     RECT rcTemp = { 0, 0, m_nWidth, m_nHeight };
//     if (!IntersectRect(&rcDst, &rcTemp, prc))
//         return;
// 
//     BITMAP  bm;
//     ::GetObject(m_hCacheBitmap, sizeof(bm), &bm);
//     byte* pBits = (byte*)bm.bmBits;
// 
//     pBits = (pBits + (rcDst.top * bm.bmWidthBytes + prc->left*4));
//     int nSize = (rcDst.right-rcDst.left)*4;
//     for (int y = rcDst.top; y < rcDst.bottom; y++)
//     {
//         int* p = (int*)pBits;
//         for (int x = rcDst.left; x < rcDst.right; x++, p++)
//         {
//             if (*p == dwTransparent)
//                 *p = 0;
//         }
//         pBits += bm.bmWidthBytes;
//     }
// }

void  CacheBitmap::AdjustWH(int nWidth, int nHeight, int& nLastWidth, int& nLastHeight)
{
#if 1
    nLastWidth = nWidth;
    nLastHeight = nHeight;
#else
//     nLastWidth = CalcMin2(nWidth);  // ��pitch����2��n�η����ӿ춨λ
//     nLastHeight = nHeight;
#endif
}

// ����2��ƽ��
int  CacheBitmap::CalcMin2(int n)
{
    int test = 1;
    for (int i = 1; i <= 20; i++)
    {
        test <<= 1;
        if (n <= test)
            return test;
    }

    return n;
}

}