
#include "stdafx.h"
#include "image.h"
namespace UI
{

Image::CInitGDIPlus		Image::s_initGDIPlus;
CDCCache			    Image::s_cache;


// 将一张图片的边缘渐变化，用于修改一张要作为窗口皮肤的图片，使边缘部分与背景颜色融合
void  Image::AlphaEdge(int nEdge, byte nSpeed)
{
    // 将自己转成带alpha channel的格式
    if (m_nBPP != 32)
    {
        Image alphaimage;
        alphaimage.Create(m_nWidth, m_nHeight, 32, createAlphaChannel);
        HDC hDC = alphaimage.GetDC();
        this->BitBlt(hDC,0,0);
        alphaimage.ReleaseDC();
        alphaimage.SetAlpha(255);
        this->Destroy();
        *this = alphaimage;
        alphaimage.Detach();
    }

    if (nEdge & ImageAlphaEdge_Bottom)
    {
        byte* p = (byte*)m_pBits + (m_nHeight-1)*m_nPitch;
        int  bBytesPerline = abs(m_nPitch);

        int alpha = 0;
        for (int y = m_nHeight; y > 0 && alpha <= 255; y--, alpha+=nSpeed)
        {
            for (int x = 0; x < bBytesPerline; x++)
            {
                p[x] = p[x]*(byte)alpha >> 8;
            }

            p -= m_nPitch;
        }
    }
    if (nEdge & ImageAlphaEdge_Top)
    {
        byte* p = (byte*)m_pBits;
        int  bBytesPerline = abs(m_nPitch);

        int alpha = 0;
        for (int y = 0; y < m_nHeight && alpha <= 255; y++, alpha+=nSpeed)
        {
            for (int x = 0; x < bBytesPerline; x++)
            {
                p[x] = p[x]*(byte)alpha >> 8;
            }

            p += m_nPitch;
        }
    }

    if (nEdge & ImageAlphaEdge_Left)
    {
        byte* p = (byte*)m_pBits;
        int  bBytesPerline = abs(m_nPitch);

        for (int y = 0; y < m_nHeight; y++)
        {
            int alpha = 0;
            for (int x = 0; x < bBytesPerline && alpha <= 255; alpha+=nSpeed)
            {
                p[x] = p[x]*(byte)alpha >> 8; x++;
                p[x] = p[x]*(byte)alpha >> 8; x++;
                p[x] = p[x]*(byte)alpha >> 8; x++;
                p[x] = p[x]*(byte)alpha >> 8; x++;
            }

            p += m_nPitch;
        }
    }

    if (nEdge & ImageAlphaEdge_Right)
    {
        byte* p = (byte*)m_pBits;
        int  bBytesPerline = abs(m_nPitch);

        for (int y = 0; y < m_nHeight; y++)
        {
            int alpha = 0;
            for (int x = bBytesPerline-1; x > 0 && alpha <= 255; alpha+=nSpeed)
            {
                p[x] = p[x]*(byte)alpha >> 8;  x--;
                p[x] = p[x]*(byte)alpha >> 8;  x--;
                p[x] = p[x]*(byte)alpha >> 8;  x--;
                p[x] = p[x]*(byte)alpha >> 8;  x--;
            }

            p += m_nPitch;
        }
    }
}



// 注：已被Util::FixGdiAlpha取代，但该函数仍然可以使用
//
// 将nAlphaIsZero==1的位置重新设置alpha=0（配置SetAlpha使用）
// 将其它位置alpha设置为255。 用于richedit在分层窗口上的绘制
bool Image::FixGDIAlphaChannel(int nAlphaIsZero)
{
    if (m_nBPP != 32)
        return false;

    BYTE* pImageBits = (BYTE*)m_pBits;
    for (int row = 0; row < m_nHeight; row ++)
    {
        for (int i = 0; i < m_nWidth; i ++)
        {
            int nAlphaIndex = 4*i+3;
            int nAlpha = pImageBits[nAlphaIndex];
            if (nAlphaIsZero == nAlpha)
            {
                pImageBits[nAlphaIndex] = 0;
            }
            else if (0 == nAlpha)
            {
                pImageBits[nAlphaIndex] = 255;
            }
        }
        pImageBits+= m_nPitch;
    }

    return true;
}

DWORD  Image::GetAverageColor(RECT* prc)
{
    if (NULL == prc)
        return 0;
    if (m_nBPP != 24 && m_nBPP != 32)   // 仅支持24、32位的图片
        return 0;

    if (NULL == m_hBitmap)
        return 0;

    int nWidth = prc->right-prc->left;
    int nHeight = prc->bottom-prc->top;

    int nCount = nWidth*nHeight;
    if (0 == nCount)
        return 0;

    BYTE* pTemp = (BYTE*)m_pBits + prc->top*m_nPitch;

    int   bytesperpx    = m_nBPP>>3;
 //   int   bytesperline   = bytesperpx*nWidth;  /*abs(m_nPitch);*/ // 注：由于位于一行要求是4的位置，可能导致当宽度为奇数数，后面会补充无用的位。因此这里不能直接用m_nPitch
    bool  bHaveAlphaChannel = GetBPP() == 32;

    LONGLONG  a = 0, g = 0, b = 0, r = 0;
    if (bHaveAlphaChannel)
    {
        for (int row = prc->top; row < prc->bottom; row ++)
        {
            int nLeftPos = prc->left*bytesperpx;
            int nRightPos = prc->right*bytesperpx;
            for( int i = nLeftPos; i < nRightPos; i += bytesperpx)
            {
                b += pTemp[i];
                g += pTemp[i+1];
                r += pTemp[i+2];
                a += pTemp[i+3];
            }
            pTemp += m_nPitch;
        }
    }
    else
    {
        for (int row = prc->top; row < prc->bottom; row ++ )
        {
            int nLeftPos = prc->left*bytesperpx;
            int nRightPos = prc->right*bytesperpx;
            for( int i = nLeftPos; i < nRightPos; i += bytesperpx)
            {
                b += pTemp[i];
                g += pTemp[i+1];
                r += pTemp[i+2];
            }
            pTemp += m_nPitch;
        }
    }

    a = a/nCount;
    r = r/nCount;
    g = g/nCount;
    b = b/nCount;

    return ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)|(((DWORD)(BYTE)(a))<<24)));
}

// 获取图像的平均色值
DWORD Image::GetAverageColor()
{
    RECT  rc = {0, 0, m_nWidth, m_nHeight};
    return GetAverageColor(&rc);

//     if (m_nBPP != 24 && m_nBPP != 32)   // 仅支持24、32位的图片
//         return 0;
// 
//     if (NULL == m_hBitmap)
//         return 0;
// 
//     int nCount = m_nWidth*m_nHeight;
//     if (0 == nCount)
//         return 0;
// 
//     BYTE* pTemp = (BYTE*)m_pBits;
//     int   bytesperpx    = m_nBPP>>3;
//     int   bytesperline   = bytesperpx*m_nWidth;  /*abs(m_nPitch);*/ // 注：由于位于一行要求是4的位置，可能导致当宽度为奇数数，后面会补充无用的位。因此这里不能直接用m_nPitch
//     bool  bHaveAlphaChannel = GetBPP() == 32;
// 
//     LONGLONG  a = 0, g = 0, b = 0, r = 0;
//     if (bHaveAlphaChannel)
//     {
//         for (int row = 0; row < m_nHeight; row ++ )
//         {
//             for( int i = 0; i < bytesperline; i += bytesperpx )
//             {
//                 b += pTemp[i];
//                 g += pTemp[i+1];
//                 r += pTemp[i+2];
//                 a += pTemp[i+3];
//             }
//             pTemp += m_nPitch;
//         }
//     }
//     else
//     {
//         for (int row = 0; row < m_nHeight; row ++ )
//         {
//             for( int i = 0; i < bytesperline; i += bytesperpx )
//             {
//                 b += pTemp[i];
//                 g += pTemp[i+1];
//                 r += pTemp[i+2];
//             }
//             pTemp += m_nPitch;
//         }
//     }
// 
//     a = a/nCount;
//     r = r/nCount;
//     g = g/nCount;
//     b = b/nCount;
// 
//     return ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)|(((DWORD)(BYTE)(a))<<24)));
}


HBITMAP Image::CopyImageByRect(RECT* prc)
{
    if (NULL == prc)
        return NULL;

    int nWidth = prc->right - prc->left;
    int nHight = prc->bottom - prc->top;

    Image image;
    image.Create(nWidth, nHight, 32, Image::createAlphaChannel);

    HDC hDC = image.GetDC();
    this->BitBlt(hDC, 0,0, nWidth, nHight, prc->left, prc->top);
    image.ReleaseDC();

    HBITMAP hRetValue = image.Detach();
    return hRetValue;
}

// 将数据全部清0
void  Image::Clear()
{
    assert(m_hBitmap);

    if ( m_nPitch < 0)  // 倒序
    {
        int nBitsSize = (m_nPitch * m_nHeight);
        byte* pDstBits = (byte*)m_pBits  + (nBitsSize - m_nPitch);
        ZeroMemory(pDstBits, -nBitsSize);
    }
    else
    {
        ZeroMemory(m_pBits, m_nPitch * m_nHeight);
    }
}


// 设置该图片中所有位置的alpha值。该图一般是一个临时图片，用于在分层窗口上使用HDC进行绘制
bool  Image::SetAlpha(byte nAlpha)
{
    assert(m_hBitmap);
    if (m_nBPP != 32)
        return false;

    BYTE* pImageBits = (BYTE*)m_pBits;
    for (int row = 0; row < m_nHeight; row ++)
    {
        for (int i = 0; i < m_nWidth; i ++)
        {
            pImageBits[4*i+3] = nAlpha;
        }
        pImageBits+= m_nPitch;
    }

    return true;
}


// 修改图片的透明度
bool Image::ModifyAlpha(ImageData* pOriginImageData, byte nAlphaPercent)
{
    assert(m_hBitmap);
    if (m_nBPP != 32)
        return false;

    if (NULL == pOriginImageData)
        return false;

    BYTE* pTemp = pOriginImageData->m_pScan0;
    if (NULL == pTemp)
        return false;

    BYTE* pNewImageBits = (BYTE*)m_pBits;
    int   bytesperpx    = m_nBPP>>3;
    int   bytesperline   = bytesperpx*m_nWidth;  /*abs(m_nPitch);*/ // 注：由于位于一行要求是4的位置，可能导致当宽度为奇数数，后面会补充无用的位。因此这里不能直接用m_nPitch

    for (int row = 0; row < m_nHeight; row ++)
    {
        for (int i = 0; i < bytesperline; i += bytesperpx)
        {
            BYTE  a = pTemp[i+3];
            a = a * nAlphaPercent / 100;

            pNewImageBits[i+3] = a;
            pNewImageBits[i]   = pTemp[i]  *a/255;
            pNewImageBits[i+1] = pTemp[i+1]*a/255;
            pNewImageBits[i+2] = pTemp[i+2]*a/255;
        }

        pNewImageBits += m_nPitch;
        pTemp += pOriginImageData->m_nStride;
    }
    return true;
}


// libo add 20120401 增加图像列表绘制方法
bool Image::ImageList_Draw(HDC hDestDC, int x, int y, int col, int row, int cx, int cy )
{
    assert(m_hBitmap);
    assert( col >= 0 );
    assert( row >= 0 );
    assert( cx >= 0 );
    assert( cy >= 0 );

    int xSrc = col * cx;
    int ySrc = row * cy;

    if( xSrc > m_nWidth )
        return false;
    if( ySrc > m_nHeight )
        return false;

    this->Draw(hDestDC, x, y, cx, cy, xSrc, ySrc, cx, cy );

    return true;
}


// libo 20120318 将DIB转换成DDB
// hMemDC
//		[in]	要用于兼容DDB的DC
//
bool Image::ChangeDIB2DDB(HDC hMemDC)
{
    assert( NULL != m_hBitmap );
    assert( NULL != hMemDC );

    if( false == m_bIsDIBSection )
        return true;

    HBITMAP hDDB = ::CreateCompatibleBitmap(hMemDC,this->GetWidth(),this->GetHeight());
    HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hDDB);

    this->BitBlt(hMemDC, 0,0 );
    ::SelectObject(hMemDC, hOldBitmap);

    this->Destroy();
    this->Attach(hDDB);

    return true;
}



//
//	根据参数中提供的原始数据pSaveBits，将自己偏移wNewH色调
//
bool ChangeColorHue(BYTE& R, BYTE& G, BYTE& B, short h, bool bOffsetOrReplace)
{
    if( R==G && G==B )  // 灰色系不能改变它的色调，永远为160
        return false;

    Color c(R,G,B,255);

    HSL hsl;
    c.GetHSL(hsl);

    if (bOffsetOrReplace)
        hsl.hue += h;
    else
        hsl.hue = h;

    while(hsl.hue < MIN_HUE_VALUE)
        hsl.hue += MAX_HUE_VALUE;
    while (hsl.hue >= MAX_HUE_VALUE)
        hsl.hue -= MAX_HUE_VALUE;
    
    c.SetHSL(hsl);

    R = c.r;
    G = c.g;
    B = c.b;

    return true;
}

void ChangeColorLuminance(BYTE& R, BYTE& G, BYTE& B, short l, float dL)  // dL = l/100;
{
    if (l > 0)  // 相当于是在背景图上面盖了一张全白的图片（alpha为dL)
    {  
        R = R + (BYTE)((255 - R) * dL);  
        G = G + (BYTE)((255 - G) * dL);  
        B = B + (BYTE)((255 - B) * dL);  
    }  
    else if (l < 0)  // 相当于是在背景图上面盖了一张全黑的图片(alpha为dL)
    {  
        R = R + (BYTE)(R * dL);  
        G = G + (BYTE)(G * dL);   
        B = B + (BYTE)(B * dL);  
    }  
#define CHECK_RGB_RANGE(x)  \
    if (x>255) x = 255; \
    if (x<0)   x = 0;

    CHECK_RGB_RANGE(R);
    CHECK_RGB_RANGE(G);
    CHECK_RGB_RANGE(B);
}

bool ChangeColorHueAndSaturation(BYTE& R, BYTE& G, BYTE& B, short h, bool bOffsetOrReplace, short s, float dS)
{
    if( R==G && G==B )  // 灰色系不能改变它的色调，永远为160
        return false;

    Color c(R,G,B,255);

    HSL hsl;
    c.GetHSL(hsl);

    // hue
    if (bOffsetOrReplace)
        hsl.hue += h;
    else
        hsl.hue = h;

    while(hsl.hue < MIN_HUE_VALUE)
        hsl.hue += MAX_HUE_VALUE;
    while (hsl.hue >= MAX_HUE_VALUE)
        hsl.hue -= MAX_HUE_VALUE;

    // saturation
    if (s > 0)
        hsl.saturation = (hsl.saturation / (1-dS));
    else
        hsl.saturation = (hsl.saturation * (1+dS));

    if(hsl.saturation <= MIN_SATURATION_VALUE)
        hsl.saturation = 0, hsl.hue = 0;  // 灰色系了

    if (hsl.saturation > MAX_SATURATION_VALUE)
        hsl.saturation = MAX_SATURATION_VALUE;


    c.SetHSL(hsl);
    R = c.r;
    G = c.g;
    B = c.b;

    return true;
}
bool ChangeColorSaturation(BYTE& R, BYTE& G, BYTE& B, short s, float dS)
{
    if( R==G && G==B )  // 灰色系不能改变它的色调，永远为160
        return false;

    Color c(R,G,B,255);

    HSL hsl;
    c.GetHSL(hsl);

    // saturation
    if (s > 0)
        hsl.saturation = (hsl.saturation / (1-dS));
    else
        hsl.saturation = (hsl.saturation * (1+dS));

    if(hsl.saturation <= MIN_SATURATION_VALUE)
        hsl.saturation = 0, hsl.hue = 0;  // 灰色系了

    if (hsl.saturation > MAX_SATURATION_VALUE)
        hsl.saturation = MAX_SATURATION_VALUE;

    c.SetHSL(hsl);
    R = c.r;
    G = c.g;
    B = c.b;

    return true;
}

bool ChangeColorHLS(BYTE& R, BYTE& G, BYTE& B, short h, short l , short s, int nFlag )
{
    bool bChangeH = nFlag & CHANGE_SKIN_HLS_FLAG_H ? true:false;
    bool bChangeL = nFlag & CHANGE_SKIN_HLS_FLAG_L ? true:false;
    bool bChangeS = nFlag & CHANGE_SKIN_HLS_FLAG_S ? true:false;
    bool bSetHueMode = nFlag & CHANGE_SKIN_HLS_FALG_REPLACE_MODE ? false:true;
    if (l == 0)
        bChangeL = false;
    if (s == 0)
        bChangeS = false;

    if(false == bChangeH && false == bChangeL && false == bChangeS)
        return false;

    float dL = 0, ds = 0;
    if (bChangeL)
        dL = (float)(l/100.0); 
    if (bChangeS)
        ds = (float)(s/100.0);

    if (bChangeL)
        ChangeColorLuminance(R,G,B,l,dL);

    if (bChangeH && bChangeS)
    {
        ChangeColorHueAndSaturation(R,G,B,h,bSetHueMode,s,ds);
    }
    else
    {
        if (bChangeH)
            ChangeColorHue(R,G,B,h,bSetHueMode);
        if (bChangeS)
            ChangeColorSaturation(R,G,B,s,ds);
    }
    return true;
}

bool Image::ChangeHSL(const ImageData* pOriginImageData, short h, short s, short l, int nFlag)
{
    // 如果pOriginImageData为NULL,则直接修改当前图片
    // 取出自己的imagedata
    ImageData tempImageData;
    if (NULL == pOriginImageData)
    {
        if (!this->SaveBits(&tempImageData, 0))
            return false;
        pOriginImageData = &tempImageData;
    }

    BYTE* pTemp = pOriginImageData->m_pScan0;
    if (NULL == pTemp)
        return false;

    if (m_nBPP != pOriginImageData->m_nbpp)
    {
        assert(0);
        return false;
    }

    bool bChangeH = nFlag & CHANGE_SKIN_HLS_FLAG_H ? true:false;
    bool bChangeL = nFlag & CHANGE_SKIN_HLS_FLAG_L ? true:false;
    bool bChangeS = nFlag & CHANGE_SKIN_HLS_FLAG_S ? true:false;
    bool bSetHueMode = nFlag & CHANGE_SKIN_HLS_FALG_REPLACE_MODE ? true:false;

    if (l == 0)
        bChangeL = false;
    if (s == 0)
        bChangeS = false;
    if (h == 0 && !bSetHueMode)
        bChangeH = false;

    // 		if(false == bChangeH && false == bChangeL && false == bChangeS) // 有可能是还原操作，因此不能直接退出 
    // 			return false;

    BYTE* pNewImageBits = (BYTE*)m_pBits;
    int   bytesperpx    = m_nBPP>>3;
    int   bytesperline   = bytesperpx*m_nWidth;  /*abs(m_nPitch);*/ // 注：由于位于一行要求是4的位置，可能导致当宽度为奇数数，后面会补充无用的位。因此这里不能直接用m_nPitch
    bool  bHaveAlphaChannel = GetBPP() == 32;

    float dL = 0, ds = 0;
    if (bChangeL)
        dL = (float)(l/100.0);   // 避免在循环中重复计算该值
    if (bChangeS)
        ds = (float)(s/100.0);

    for (int row = 0; row < m_nHeight; row ++)
    {
        for (int i = 0; i < bytesperline; i += bytesperpx)
        {
            BYTE B = pTemp[i];
            BYTE G = pTemp[i+1];
            BYTE R = pTemp[i+2];
            BYTE A = pTemp[i+3];

            // 首先要反预乘，计算出结果后，再预乘。这样在alphablend时才不会溢出
            if (A != 0 && A != 255)
            {
                B = (BYTE)pTemp[i]*255/A;
                G = (BYTE)pTemp[i+1]*255/A;
                R = (BYTE)pTemp[i+2]*255/A;
            }

            if (bChangeL)
                ChangeColorLuminance(R,G,B,l,dL);

            if (bChangeH && bChangeS)
            {
                ChangeColorHueAndSaturation(R,G,B,h,!bSetHueMode,s,ds);
            }
            else
            {
                if (bChangeH)
                    ChangeColorHue(R,G,B,h,!bSetHueMode);
                if (bChangeS)
                    ChangeColorSaturation(R,G,B,s,ds);
            }

            if (A != 0 && A != 255)
            {
                // 预乘
                B = B*A/255;
                G = G*A/255;
                R = R*A/255;
            }

            pNewImageBits[i]   = B;
            pNewImageBits[i+1] = G;
            pNewImageBits[i+2] = R;

            if (bHaveAlphaChannel)
            {
                pNewImageBits[i+3] = pTemp[i+3];
            }
        }

        pNewImageBits += m_nPitch;
        pTemp += pOriginImageData->m_nStride;
    }

    return true;
}

// 为了解决内存在另一个模块分配，却在其它模块释放，在这里修改为由外部自己分配内存
bool Image::SaveBits(ImageData* pImageData, int* pNeedSize)
{
    if (NULL == pImageData && NULL == pNeedSize)
        return false;

    if (m_nBPP != 24 && m_nBPP != 32)  // TODO: 暂不支持其它格式的（8位的是基于调色板的）
        return false;

    int   bytesperline = abs(m_nPitch);
    int nSize = bytesperline*m_nHeight;
    if (pNeedSize && *pNeedSize != nSize)
    {
        *pNeedSize = nSize;
        return false;
    }

    pImageData->m_nWidth = m_nWidth;
    pImageData->m_nHeight = m_nHeight;
    pImageData->m_nStride = m_nPitch;
    pImageData->m_nbpp = m_nBPP;

    // 只获取，不拷贝
    if (NULL == pImageData->m_ptr)
    {
        if (pImageData->m_bNeedDeletePtr)
            return false;

        pImageData->m_pScan0 = (byte*)m_pBits;
        pImageData->m_ptr = (byte*)m_pBits;
        if (m_nPitch < 0)
            pImageData->m_ptr += ((m_nHeight-1)*m_nPitch);
    }
    else
    {
        pImageData->m_pScan0 = pImageData->m_ptr;
        if (m_nPitch < 0)
            pImageData->m_pScan0 += ((m_nHeight-1)*bytesperline);

        // 内存拷贝
        BYTE* pThisBits = (BYTE*)m_pBits;
        BYTE* pTemp = pImageData->m_pScan0;
        for (int row = 0; row < m_nHeight; row ++ )
        {
            memcpy(pTemp, pThisBits, bytesperline);
            pThisBits += m_nPitch;
            pTemp += m_nPitch;
        }
    }
    return true;
}

void Image::RestoreBits( ImageData* pImageData )
{
    // TODO
}

}