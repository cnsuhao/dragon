#include "stdafx.h"
#include "antiwrap.h"
#include "UISDK/Kernel/Src/UIObject/Window/customwindow.h"
#include "UISDK/Kernel/Src/RenderLayer/renderchain.h"

namespace UI
{

AntiWindowWrap::AntiWindowWrap()
{

}
AntiWindowWrap::~AntiWindowWrap()
{

}

void  AntiWindowWrap::Init(ICustomWindow* pWnd)
{
    __super::Init(pWnd);

    IMapAttribute* pMapAttrib = NULL;
    m_pWindow->GetMapAttribute(&pMapAttrib);
    if (pMapAttrib)
    {
        const TCHAR* szText = pMapAttrib->GetAttr(XML_WINDOW_TRANSPARENT_ANTI_9REGION, false);
        Util::TranslateImage9Region(szText, &m_9region, XML_SEPARATOR);

        SAFE_RELEASE(pMapAttrib);
    }
}

void  AntiWindowWrap::UpdateRgn()
{
    if (m_9region.IsAll0())
        return ;

    CRect rc;
    ::GetWindowRect(m_hWnd, &rc);
    int w = rc.Width();
    int h = rc.Height();

    int rl = m_9region.left;
    int rt = m_9region.top;
    int rr = m_9region.right;
    int rb = m_9region.bottom;

    RECT rect1, rect2, rect3;

    vector<RECT>  vRectRgnData;
    switch (m_9region.topleft)
    {
    case 0:
        break;

    case 1:
        {
            ::SetRect(&rect1, 0, 0, 1, 1);
            ::OffsetRect(&rect1, rl, rt);
            vRectRgnData.push_back(rect1);
        }
        break;

    case 2:
        {
            ::SetRect(&rect1, 0, 0, 2, 1);
            ::SetRect(&rect2, 0, 1, 1, 2);

            ::OffsetRect(&rect1, rl, rt);
            vRectRgnData.push_back(rect1);
            ::OffsetRect(&rect2, rl, rt);
            vRectRgnData.push_back(rect2);
        }
        break;

    case 3:
    default:
        {
            ::SetRect(&rect1, 0, 0, 3, 1);
            ::SetRect(&rect2, 0, 1, 2, 2);
            ::SetRect(&rect3, 0, 2, 1, 3);

            ::OffsetRect(&rect1, rl, rt);
            vRectRgnData.push_back(rect1);
            ::OffsetRect(&rect2, rl, rt);
            vRectRgnData.push_back(rect2);
            ::OffsetRect(&rect3, rl, rt);
            vRectRgnData.push_back(rect3);
        }
        break;
    }

    switch (m_9region.topright)
    {
    case 0:
        break;

    case 1:
        {
            ::SetRect(&rect1, w-1, 0, w, 1); 
            ::OffsetRect(&rect1, -rr, rt);
            vRectRgnData.push_back(rect1);
        }
        break;

    case 2:
        {
            ::SetRect(&rect1, w-2, 0, w, 1);
            ::SetRect(&rect2, w-1, 1, w, 2);

            OffsetRect(&rect1, -rr, rt);
            vRectRgnData.push_back(rect1);
            OffsetRect(&rect2, -rr, rt);
            vRectRgnData.push_back(rect2);
        }
        break;

    case 3:
    default:
        {
            ::SetRect(&rect1, w-3, 0, w, 1);
            ::SetRect(&rect2, w-2, 1, w, 2);
            ::SetRect(&rect3, w-1, 2, w, 3);

            ::OffsetRect(&rect1, -rr, rt);
            vRectRgnData.push_back(rect1);
            ::OffsetRect(&rect2, -rr, rt);
            vRectRgnData.push_back(rect2);
            ::OffsetRect(&rect3, -rr, rt);
            vRectRgnData.push_back(rect3);
        }
        break;
    }

    switch (m_9region.bottomleft)
    {
    case 0:
        break;

    case 1:
        {
            ::SetRect(&rect1, 0, h-1, 1, h); //  {0, 0};
            OffsetRect(&rect1, rl, -rb);
            vRectRgnData.push_back(rect1);
        }
        break;

    case 2:
        {
            ::SetRect(&rect1, 0, h-2, 1, h-1);
            ::SetRect(&rect2, 0, h-1, 2, h);

            OffsetRect(&rect1, rl, -rb);
            vRectRgnData.push_back(rect1);
            OffsetRect(&rect2, rl, -rb);
            vRectRgnData.push_back(rect2);
        }
        break;

    case 3:
    default:
        {
            ::SetRect(&rect1, 0, h-1, 3, h);
            ::SetRect(&rect2, 0, h-2, 2, h-1);
            ::SetRect(&rect3, 0, h-3, 1, h-2);

            ::OffsetRect(&rect1, rl, -rb);
            vRectRgnData.push_back(rect1);
            ::OffsetRect(&rect2, rl, -rb);
            vRectRgnData.push_back(rect2);
            ::OffsetRect(&rect3, rl, -rb);
            vRectRgnData.push_back(rect3);
        }
        break;
    }

    switch (m_9region.bottomright)
    {
    case 0:
        break;

    case 1:
        {
            ::SetRect(&rect1, w-1, h-1, w, h);
            OffsetRect(&rect1, -rr, -rb);
            vRectRgnData.push_back(rect1);
        }
        break;

    case 2:
        {
            ::SetRect(&rect1, w-1, h-2, w, h-1);
            ::SetRect(&rect2, w-2, h-1, w, h);

            OffsetRect(&rect1, -rr, -rb);
            vRectRgnData.push_back(rect1);
            OffsetRect(&rect2, -rr, -rb);
            vRectRgnData.push_back(rect2);
        }
        break;

    case 3:
    default:
        {
            ::SetRect(&rect1, w-3, h-1, w, h);
            ::SetRect(&rect2, w-2, h-2, w, h-1);
            ::SetRect(&rect3, w-1, h-3, w, h-2);

            ::OffsetRect(&rect1, -rr, -rb);
            vRectRgnData.push_back(rect1);
            ::OffsetRect(&rect2, -rr, -rb);
            vRectRgnData.push_back(rect2);
            ::OffsetRect(&rect3, -rr, -rb);
            vRectRgnData.push_back(rect3);
        }
        break;
    }

    HRGN hRgn2Delete =  CreateRgnByVectorRect(vRectRgnData);

    CRect rectRgn( 0, 0, w, h);
    rectRgn.DeflateRect(rl, rt, rr, rb);

    HRGN  hRgn = CreateRectRgnIndirect(&rectRgn);
    CombineRgn(hRgn, hRgn, hRgn2Delete, RGN_DIFF);
    ::SetWindowRgn(m_hWnd, hRgn, TRUE);
    DeleteObject(hRgn);
    DeleteObject(hRgn2Delete);
}
void  AntiWindowWrap::Enable(bool b)
{
	if (false == b)  // 取消当前异形
	{
		SetWindowRgn(m_hWnd, NULL, FALSE);
	}
}

}