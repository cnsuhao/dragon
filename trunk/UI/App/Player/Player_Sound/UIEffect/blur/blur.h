#pragma once

class Blur
{
public:
    // output = (up + left + right + down + middle)/5;
    static void  Blur32(HBITMAP hBitmap, bool /*bTopDown*/)
    {
        // 默认是top down
        BITMAP bm = {0};
        GetObject(hBitmap, sizeof(bm), &bm);
        if (bm.bmBitsPixel != 32)
            return;

        DWORD* pBits = (DWORD*) bm.bmBits;

#define GETPIXEL32(x,y)  \
    *(pBits + ((y)*bm.bmWidth) + (x))
#define SETPIXEL32(x,y,c)  \
    *(pBits + ((y)*bm.bmWidth) + (x)) = c;

        COLORREF cLeft = 0;
        COLORREF cCur = GETPIXEL32(0,0);
        COLORREF cRight = 0;
        COLORREF cUp = 0;
        COLORREF cDown = 0;

        // 定义波形消失的速度 
#ifdef WAVE_SPEED_FAST
        const float fDiv = 6.1f;
#elif defined WAVE_SPEED_MEDIUM
        const float fDiv = 5.5f;
#else
        const float fDiv = 5.08f;
#endif

        const int fixpoint = 16;
        const int divShift = (int)((1.0f/fDiv) * 65536);  // 除法 改成定点数乘法

        BYTE r, g, b;
        int sumr, sumg, sumb;

        int nRight = bm.bmWidth - 1;
        int nBottom = bm.bmHeight - 1;
        for (int y = 1; y < nBottom; y++)
        {
            cLeft = 0;
            for (int x = 0; x < nRight; x++) 
            {
                cRight = GETPIXEL32(x+1, y);
                cUp = GETPIXEL32(x, y-1);
                cDown = GETPIXEL32(x, y+1);
 
                sumr = GetRValue(cCur) + GetRValue(cLeft) + GetRValue(cRight) + GetRValue(cUp) + GetRValue(cDown);
                sumg = GetGValue(cCur) + GetGValue(cLeft) + GetGValue(cRight) + GetGValue(cUp) + GetGValue(cDown);
                sumb = GetBValue(cCur) + GetBValue(cLeft) + GetBValue(cRight) + GetBValue(cUp) + GetBValue(cDown);

                r = (BYTE)((sumr * divShift) >> fixpoint);
                g = (BYTE)((sumg * divShift) >> fixpoint);
                b = (BYTE)((sumb * divShift) >> fixpoint);

                SETPIXEL32(x, y, RGB(r,g,b)); 
                cLeft = cCur;
                cCur = cRight;
            }
        }   


        // 将四周全部设置为0，懒得去管什么边界了，简单处理

        // top
        memset(bm.bmBits, 0, bm.bmWidthBytes);

        // down
        memset((byte*)bm.bmBits + (bm.bmHeight-1)*bm.bmWidthBytes, 0, bm.bmWidthBytes);

        // left/right
        int*  pLeft = (int*)bm.bmBits;
        int*  pRight = (int*)pLeft + (bm.bmWidth-1);
        for (int i = 0; i < bm.bmHeight; i++)
        {
            *pLeft = 0;
            *pRight = 0;

            pLeft += bm.bmWidth;
            pRight += bm.bmWidth;
        }
    }



   

};