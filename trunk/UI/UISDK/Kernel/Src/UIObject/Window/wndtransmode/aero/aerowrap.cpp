#include "stdafx.h"
#include "aerowrap.h"
#include "UISDK\Kernel\Src\UIObject\Window\customwindow.h"
#include "UISDK/Kernel/Inc/Base/inc.h"

namespace UI
{
AeroWindowWrap::AeroWindowWrap()
{
    m_pDWM = DwmHelper::GetInstance();
    m_pAeroDisableMode = NULL;
    m_pIAeroWndTransMode = NULL;
    m_eAeroDisableMode = WINDOW_TRANSPARENT_MODE_LAYERED;
    m_eMode = AERO_MODE_TRANSPARENT;
    ::SetRect(&blur.m_regionBlur, -1, -1, -1, -1);
    blur.m_hrgnBlurRgn = NULL;
}
AeroWindowWrap::~AeroWindowWrap()
{
    m_pDWM = NULL;
    SAFE_DELETE(m_pIAeroWndTransMode);
    SAFE_RELEASE(m_pAeroDisableMode);
    SAFE_DELETE_GDIOBJECT(blur.m_hrgnBlurRgn);
}

void  AeroWindowWrap::Init(ICustomWindow* pWnd)
{
    __super::Init(pWnd);

    IMapAttribute* pMapAttrib = m_pWindow->m_pIMapAttributeRemain;
    if (pMapAttrib)
    {
        const TCHAR* szText = m_pWindow->m_pIMapAttributeRemain->GetAttr(XML_WINDOW_TRANSPARENT_TYPE_AERO_DISABLE, true);
        if (szText)
            m_eAeroDisableMode = GetTransparentModeTypeFromAttr(szText);

        szText = pMapAttrib->GetAttr(XML_WINDOW_TRANSPARENT_AERO_MODE, false);
        if (szText)
        {
            if (0 == _tcscmp(szText, XML_WINDOW_TRANSPARENT_AERO_MODE_BLUR))
            {
                m_eMode = AERO_MODE_BLUR;
            }
            else
            {
                m_eMode = AERO_MODE_TRANSPARENT;
            }
        }

        if (AERO_MODE_TRANSPARENT == m_eMode)
        {
            szText = pMapAttrib->GetAttr(XML_WINDOW_TRANSPARENT_AERO_TRANS_MARGINS, false);
            if (szText)
            {
				// 注： 如果这个值过小，会导致窗口四个圆角变形！建议至少为5? 7?
                RECT rc;
                if (Util::TranslateRECT(szText, &rc, XML_SEPARATOR))
                {
                    trans.m_margins.cxLeftWidth = rc.left;
                    trans.m_margins.cxRightWidth = rc.right;
                    trans.m_margins.cyTopHeight = rc.top;
                    trans.m_margins.cyBottomHeight = rc.bottom;
#ifdef _DEBUG
					UIASSERT(rc.left == -1 || rc.left >= 5);
					UIASSERT(rc.right == -1 || rc.right >= 5);
					UIASSERT(rc.top == -1 || rc.top >= 5);
					UIASSERT(rc.bottom == -1 || rc.bottom >= 5);
#endif
                }
            }
        }
        else
        {
            szText = pMapAttrib->GetAttr(XML_WINDOW_TRANSPARENT_AERO_BLUR_REGION, false);
            if (szText)
            {
                Util::TranslateRECT(szText, &blur.m_regionBlur, XML_SEPARATOR);
            }
        }
    }
}

LRESULT  AeroWindowWrap::_OnDwmCompositionChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    Enable(true);
    m_pWindow->m_bNeedToSetWindowRgn = true;
    m_pWindow->UpdateObject();

    bHandled = FALSE;
    return 0;
}

LRESULT  AeroWindowWrap::_OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (m_eMode == AERO_MODE_TRANSPARENT)
	{
		if (wParam)
		{
			bHandled = TRUE;
			return 0;
		}
	}
	bHandled = FALSE;
	return 0;
}

IAeroWindowWrap*  AeroWindowWrap::GetIAeroWndTransMode()
{
    if (!m_pIAeroWndTransMode)
        m_pIAeroWndTransMode = new IAeroWindowWrap(this);

    return m_pIAeroWndTransMode;
}
void*  AeroWindowWrap::QueryInterface(const IID* pIID)
{
    if (IsEqualIID(*pIID, uiiidof(IAeroWindowWrap)))  
    {   
        SetMsgHandled(TRUE);
        return (void*)GetIAeroWndTransMode();                 
    }                                              
    SetMsgHandled(FALSE);         
    return 0;
}

AERO_MODE  AeroWindowWrap::GetAeroMode()
{
    return m_eMode;
}   
void  AeroWindowWrap::GetBlurRegion(CRegion4* pregion)
{
    if (pregion && m_eMode == AERO_MODE_BLUR)
    {
        pregion->CopyRect(&blur.m_regionBlur);
    }
}

void  AeroWindowWrap::SetTransparentMargins(RECT* prcMargin)
{
	if (m_eMode != AERO_MODE_TRANSPARENT)
		return;

	trans.m_margins.cxLeftWidth = prcMargin->left;
	trans.m_margins.cxRightWidth = prcMargin->right;
	trans.m_margins.cyTopHeight = prcMargin->top;
	trans.m_margins.cyBottomHeight = prcMargin->bottom;
}

void  AeroWindowWrap::Enable(bool b)
{
    if (m_pDWM->IsEnable())
    {
        if (b)
        {
            if (m_pAeroDisableMode)
            {
                m_pAeroDisableMode->Enable(false);
                SAFE_RELEASE(m_pAeroDisableMode);
            }

			if (m_eMode == AERO_MODE_TRANSPARENT)
			{
				// 注：使用自已的图片阴影+aero透明会导致一个解决不了的问题：窗口改变大小时出现黑边问题，如拖拽和最大化时
				//     为了规避这个问题，只能采用aero自带的阴影，因此得添加一个WS_THICKFRAME属性以开启阴影，再加上WS_NCCALCSIZE/WS_NCACTIVATE消息
				//     屏蔽边框的显示
				long lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
				SetWindowLong(m_hWnd, GWL_STYLE, lStyle|WS_THICKFRAME);
				SetWindowPos(m_hWnd, 0,0,0,0,0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_FRAMECHANGED);
			}
            return;
            // 由外部调用UpdateRgn即可
        }
        else
        {
            if (m_eMode == AERO_MODE_BLUR)
            {
                DwmHelper* pDwm = DwmHelper::GetInstance();
                if (pDwm->pDwmEnableBlurBehindWindow)
                {
                    DWM_BLURBEHIND blurbehind = {0};
                    blurbehind.dwFlags = DWM_BB_ENABLE;
                    blurbehind.fEnable = FALSE;
                    pDwm->pDwmEnableBlurBehindWindow(m_pWindow->m_hWnd, &blurbehind);
                }
            }
            else
            {
                if (m_pDWM->pDwmExtendFrameIntoClientArea)
                {
                    MARGINS m = {0};
                    m_pDWM->pDwmExtendFrameIntoClientArea(m_hWnd, &m);
                }

				if (m_eMode == AERO_MODE_TRANSPARENT)
				{
					long lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
					if (lStyle & WS_THICKFRAME)
					{
						SetWindowLong(m_hWnd, GWL_STYLE, lStyle & ~WS_THICKFRAME);
						SetWindowPos(m_hWnd, 0,0,0,0,0, SWP_NOZORDER|SWP_NOSIZE|SWP_NOMOVE|SWP_NOACTIVATE|SWP_FRAMECHANGED);
					}
				}
            }
        }
    }
    
    do 
    {
        if (m_pAeroDisableMode)
        {
            m_pAeroDisableMode->Enable(true);
            break;
        }

        m_pAeroDisableMode = CreateTransparentModeByType(m_eAeroDisableMode);
        if (!m_pAeroDisableMode)
            break;

        m_pAeroDisableMode->Init(m_pWindow->m_pICustomWindow);
        m_pAeroDisableMode->Enable(true);

    } while (0);


}

void  AeroWindowWrap::UpdateRgn()
{
    if (m_pAeroDisableMode)
    {
        m_pAeroDisableMode->UpdateRgn();
        return;
    }


    DwmHelper*  pDwm = DwmHelper::GetInstance();
    if (pDwm->IsEnable())
    {
        if (m_eMode == AERO_MODE_BLUR)
        {
            if (pDwm->pDwmEnableBlurBehindWindow)
            {
                DWM_BLURBEHIND blurbehind = {0};
                blurbehind.dwFlags = DWM_BB_ENABLE|DWM_BB_BLURREGION|DWM_BB_TRANSITIONONMAXIMIZED;
                blurbehind.fEnable = TRUE;

                if (blur.m_hrgnBlurRgn)
                {
                    blurbehind.hRgnBlur = blur.m_hrgnBlurRgn;
                    blurbehind.fTransitionOnMaximized = TRUE;
                    pDwm->pDwmEnableBlurBehindWindow(m_hWnd, &blurbehind);
                }
                else
                {
                    CRect  rcClient;
                    ::GetClientRect(m_pWindow->m_hWnd, &rcClient);
                    rcClient.DeflateRect(blur.m_regionBlur.left, 
                        blur.m_regionBlur.top, 
                        blur.m_regionBlur.right, 
                        blur.m_regionBlur.bottom);

                    HRGN hRgn = CreateRectRgnIndirect(&rcClient);
                    blurbehind.hRgnBlur = hRgn;
                    blurbehind.fTransitionOnMaximized = TRUE;
                    pDwm->pDwmEnableBlurBehindWindow(m_hWnd, &blurbehind);
                    DeleteObject(hRgn);
                }
            }
        }
        else if (AERO_MODE_TRANSPARENT == m_eMode)
        {
             if (m_pDWM->pDwmExtendFrameIntoClientArea)
             {
				 m_pDWM->pDwmExtendFrameIntoClientArea(m_hWnd, &trans.m_margins);
             }
        }
    }
}

WINDOW_TRANSPARENT_MODE  AeroWindowWrap::GetModeValue()
{
    if (m_pAeroDisableMode)
        return m_pAeroDisableMode->GetModeValue();

    return WINDOW_TRANSPARENT_MODE_AERO;
}

bool  AeroWindowWrap::RequireAlphaChannel()
{
    if (m_pAeroDisableMode)
        return m_pAeroDisableMode->RequireAlphaChannel();

    return true;
}
bool  AeroWindowWrap::Commit()
{
    if (m_pAeroDisableMode)
    {
        return m_pAeroDisableMode->Commit();
    }

    return false;
}

}