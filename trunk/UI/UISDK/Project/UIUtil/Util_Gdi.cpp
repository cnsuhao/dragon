#include "stdafx.h"
#include "h\util.h"

namespace UI { namespace Util
{
#ifdef UTIL_GDI
#pragma comment(lib,"Msimg32.lib")   // GradientFill

//
//	将字体的大小转换成CreateFont需要使用的字体高度
//
//	Parameter
//		nSize	
//			[in]	字体界面上显示的大小
//
//	Return
//		返回字体所对应的height;
//
int FontSize2Height(int nSize)
{
	int nHeight = 0;
	HDC hDC = ::GetDC(NULL);
	nHeight = -MulDiv(nSize, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	::ReleaseDC(NULL,hDC);
	return nHeight;
}

//
//	将字体LOGFONT.lfHeight转换成字体大小
//
//	Parameter
//		nHeight	
//			[in]	字体界面上显示的大小
//
//	Return
//		返回字体所对应的size;
//
int FontHeight2Size(int nHeight)
{
	int nSize = 0;
	HDC hDC = ::GetDC(NULL);
	nSize = -MulDiv( nHeight, 72, GetDeviceCaps(hDC, LOGPIXELSY) );  // 96
	::ReleaseDC(NULL,hDC);
	return nSize;
}

namespace UI { namespace Util {
int CALLBACK IsFontExistEnumFontsProc(
						   CONST LOGFONT *lplf,     // logical-font data
						   CONST TEXTMETRIC *lptm,  // physical-font data
						   DWORD dwType,            // font type
						   LPARAM lpData            // application-defined data
						   )
{
	BOOL* pbFind = (BOOL*)lpData;
	if (pbFind)
	{
		*pbFind = TRUE;
	}
	return 0;
}
} }

//
//  判断指定的字体在该系统中是否存在
//
//	Parameter
//		pszFaceName
//			[in]	要检测的字体名称
//
BOOL IsFontExist(const TCHAR* pszFaceName)
{
	if (NULL == pszFaceName)
		return FALSE;

	HDC hDC = GetDC(NULL);
	BOOL bFind = FALSE;
	int nRet = EnumFonts(hDC, pszFaceName, UI::Util::IsFontExistEnumFontsProc, (LPARAM)&bFind);
	::ReleaseDC(NULL, hDC);
	
	return bFind;
}

BOOL GradientFillH( HDC hDC, const RECT* prc, COLORREF colFrom, COLORREF colTo )
{
	if( NULL == prc )
		return FALSE;

	if( colFrom == colTo )
	{
		HBRUSH hBrush = ::CreateSolidBrush(colFrom);
		::FillRect(hDC,prc,hBrush);
		::DeleteObject(hBrush);

		return TRUE;
	}

	TRIVERTEX        vert[2] ;
	GRADIENT_RECT    gRect;
	vert [0] .x      = prc->left;
	vert [0] .y      = prc->top;
	vert [0] .Red    = GetRValue(colFrom) << 8;
	vert [0] .Green  = GetGValue(colFrom) << 8;
	vert [0] .Blue   = GetBValue(colFrom) << 8;
	vert [0] .Alpha  = 0x0000;

	vert [1] .x      = prc->right;
	vert [1] .y      = prc->bottom; 
	vert [1] .Red    = GetRValue(colTo) << 8;
	vert [1] .Green  = GetGValue(colTo) << 8;
	vert [1] .Blue   = GetBValue(colTo) << 8;
	vert [1] .Alpha  = 0x0000;

	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;
	GradientFill(hDC,vert,2,&gRect,1,GRADIENT_FILL_RECT_H);

	return TRUE;
}
int GradientFillV( HDC hDC, const RECT* prc, COLORREF colFrom, COLORREF colTo )
{
	if( NULL == prc )
		return FALSE;

	if( colFrom == colTo )
	{
		HBRUSH hBrush = ::CreateSolidBrush(colFrom);
		::FillRect(hDC,prc,hBrush);
		::DeleteObject(hBrush);

		return TRUE;
	}

	TRIVERTEX        vert[2] ;
	GRADIENT_RECT    gRect;
	vert [0] .x      = prc->left;
	vert [0] .y      = prc->top;
	vert [0] .Red    = GetRValue(colFrom) << 8;
	vert [0] .Green  = GetGValue(colFrom) << 8;
	vert [0] .Blue   = GetBValue(colFrom) << 8;
	vert [0] .Alpha  = 0x0000;

	vert [1] .x      = prc->right;
	vert [1] .y      = prc->bottom; 
	vert [1] .Red    = GetRValue(colTo) << 8;
	vert [1] .Green  = GetGValue(colTo) << 8;
	vert [1] .Blue   = GetBValue(colTo) << 8;
	vert [1] .Alpha  = 0x0000;

	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;
	GradientFill(hDC,vert,2,&gRect,1,GRADIENT_FILL_RECT_V);

	return TRUE;
}

// HIMETRIC units per inch (used for conversion)
#define HIMETRIC_PER_INCH 2540
// Convert Himetric along the X axis to X pixels
LONG HimetricXtoDX(LONG xHimetric, LONG xPerInch) 
{	
	if (-1 == xPerInch)
	{
		HDC hDC = GetDC(NULL);
		xPerInch = GetDeviceCaps(hDC, LOGPIXELSX); 
		ReleaseDC(NULL, hDC);
	}
	return (LONG) MulDiv(xHimetric, xPerInch, HIMETRIC_PER_INCH);
}
// Convert Himetric along the Y axis to Y pixels
LONG HimetricYtoDY(LONG yHimetric, LONG yPerInch) 
{	
	if (-1 == yPerInch)
	{
		HDC hDC = GetDC(NULL);
		yPerInch = GetDeviceCaps(hDC, LOGPIXELSY); 
		ReleaseDC(NULL, hDC);
	}
	return (LONG) MulDiv(yHimetric, yPerInch, HIMETRIC_PER_INCH);
}
// Convert Pixels on the X axis to Himetric
LONG DXtoHimetricX(LONG dx, LONG xPerInch) 
{ 
	if (-1 == xPerInch)
	{
		HDC hDC = GetDC(NULL);
		xPerInch = GetDeviceCaps(hDC, LOGPIXELSX); 
		ReleaseDC(NULL, hDC);
	}
	return (LONG) MulDiv(dx, HIMETRIC_PER_INCH, xPerInch); 
}
// Convert Pixels on the Y axis to Himetric
LONG DYtoHimetricY(LONG dy, LONG yPerInch) 
{ 
	if (-1 == yPerInch)
	{
		HDC hDC = GetDC(NULL);
		yPerInch = GetDeviceCaps(hDC, LOGPIXELSY); 
		ReleaseDC(NULL, hDC);
	}
	return (LONG) MulDiv(dy, HIMETRIC_PER_INCH, yPerInch); 
}
#endif

BOOL IsLayeredWindow(HWND hWnd)
{
	if (NULL == hWnd)
		return FALSE;

	LONG lStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
	if (lStyle & WS_EX_LAYERED)
		return TRUE;

	return FALSE;
}



BOOL FixBitmapAlpha(FixAlphaData* pData)
{
	if (!pData)
		return FALSE;

	HBITMAP hBitmap = pData->hBitmap;
	if (!hBitmap)
	{
		if (!pData->hDC)
			return FALSE;

		hBitmap = (HBITMAP)GetCurrentObject(pData->hDC, OBJ_BITMAP);
	}

	BITMAP  bm;
	GetObject(hBitmap, sizeof(BITMAP), &bm);  // bm.bmBits指针数据开始的地址，但这个地址不一定是第一行，也有可能是最后一行

	if (bm.bmBitsPixel != 32)
		return FALSE;

	RECT rcDest;
	if (pData->lprc)
	{
		RECT rcBitmap = {0, 0, bm.bmWidth, bm.bmHeight};
		if (FALSE == ::IntersectRect(&rcDest, &rcBitmap, pData->lprc))
			return FALSE;
	}
	else
	{
		SetRect(&rcDest, 0, 0, bm.bmWidth, bm.bmHeight);
	}

	BYTE* pBits = (BYTE*)bm.bmBits;
	int nPitch = bm.bmWidthBytes;
	if (!pData->bTopDownDib)
	{
		pBits = LPBYTE( pBits )+((bm.bmHeight-1)*nPitch);
		nPitch = -nPitch;
	}

	BYTE* p = NULL;
	BYTE* pEnd = NULL;
	pBits += rcDest.top*nPitch;


	switch (pData->eMode)
	{
	case SET_ALPHA_255:
		{
			for (int y = rcDest.top ; y < rcDest.bottom; y++)
			{
				p    = pBits + (rcDest.left << 2) + 3;
				pEnd = pBits + (rcDest.right << 2) + 3;

				while (p < pEnd)
				{
					*p = 0xFF;
					p += 4;
				}

				pBits += nPitch;
			}
		}
		break;

	case SET_ALPHA_255_IF_ALPHA_IS_0:
		{
			for (int y = rcDest.top ; y < rcDest.bottom; y++)
			{
				p    = pBits + (rcDest.left << 2) + 3;
				pEnd = pBits + (rcDest.right << 2) + 3;

				while (p < pEnd)
				{
					if (0 == (*p))
					{
						*p = 0xFF;
					}
					p += 4;
				}

				pBits += nPitch;
			}
		}
		break;

    case  SET_ALPHA_255_IF_RGBA_ISNOT_0:
        {
            DWORD* p4 = NULL;
            DWORD* p4End = NULL;
            for (int y = rcDest.top ; y < rcDest.bottom; y++)
            {
                p4    = (DWORD*)(pBits + (rcDest.left << 2));
                p4End = (DWORD*)(pBits + (rcDest.right << 2));

                while (p4 < p4End)
                {
                    if (0 != (*p4))
                    {
                        *p4 |= 0xFF000000;
                    }
                    p4 ++;
                }

                pBits += nPitch;
            }
        }
        break;

    case SET_ALPHA_INVERSE_0_255:
        {
            for (int y = rcDest.top ; y < rcDest.bottom; y++)
            {
                p    = pBits + (rcDest.left << 2) + 3;
                pEnd = pBits + (rcDest.right << 2) + 3;

                while (p < pEnd)
                {
                    *p = *p^0xFF;  // 异或
                    p += 4;
                }

                pBits += nPitch;
            }
        }
        break;
	}
	return TRUE;
}

}
}
