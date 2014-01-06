#include "stdafx.h"
#include "areowrap.h"
#include "UISDK\Kernel\Src\UIObject\Window\customwindow.h"
#include "UISDK/Kernel/Inc/Base/inc.h"

namespace UI
{
AreoWindowWrap::AreoWindowWrap()
{
    m_pDWM = DwmHelper::GetInstance();
    m_pAreoDisableMode = NULL;
    m_pIAreoWndTransMode = NULL;
    m_eAreoDisableMode = WINDOW_TRANSPARENT_MODE_LAYERED;
    m_eMode = AREO_MODE_TRANSPARENT;
    ::SetRect(&blur.m_regionBlur, -1, -1, -1, -1);
    blur.m_hrgnBlurRgn = NULL;
}
AreoWindowWrap::~AreoWindowWrap()
{
    m_pDWM = NULL;
    SAFE_DELETE(m_pIAreoWndTransMode);
    SAFE_RELEASE(m_pAreoDisableMode);
    SAFE_DELETE_GDIOBJECT(blur.m_hrgnBlurRgn);
}

void  AreoWindowWrap::Init(ICustomWindow* pWnd)
{
    __super::Init(pWnd);

    IMapAttribute* pMapAttrib = m_pWindow->m_pIMapAttributeRemain;
    if (pMapAttrib)
    {
        const TCHAR* szText = m_pWindow->m_pIMapAttributeRemain->GetAttr(XML_WINDOW_TRANSPARENT_TYPE_AREO_DISABLE, true);
        if (szText)
            m_eAreoDisableMode = GetTransparentModeTypeFromAttr(szText);

        szText = pMapAttrib->GetAttr(XML_WINDOW_TRANSPARENT_AREO_MODE, false);
        if (szText)
        {
            if (0 == _tcscmp(szText, XML_WINDOW_TRANSPARENT_AREO_MODE_BLUR))
            {
                m_eMode = AREO_MODE_BLUR;
            }
            else
            {
                m_eMode = AREO_MODE_TRANSPARENT;
            }
        }

        if (AREO_MODE_TRANSPARENT == m_eMode)
        {
            szText = pMapAttrib->GetAttr(XML_WINDOW_TRANSPARENT_AREO_TRANS_MARGINS, false);
            if (szText)
            {
                RECT rc;
                if (Util::TranslateRECT(szText, &rc, XML_SEPARATOR))
                {
                    trans.m_margins.cxLeftWidth = rc.left;
                    trans.m_margins.cxRightWidth = rc.right;
                    trans.m_margins.cyTopHeight = rc.top;
                    trans.m_margins.cyBottomHeight = rc.bottom;
                }
            }
        }
        else
        {
            szText = pMapAttrib->GetAttr(XML_WINDOW_TRANSPARENT_AREO_BLUR_REGION, false);
            if (szText)
            {
                Util::TranslateRECT(szText, &blur.m_regionBlur, XML_SEPARATOR);
            }
        }
    }
}

LRESULT  AreoWindowWrap::_OnDwmCompositionChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    Enable(true);
    m_pWindow->m_bNeedToSetWindowRgn = true;
    m_pWindow->UpdateObject();

    bHandled = FALSE;
    return 0;
}

IAreoWindowWrap*  AreoWindowWrap::GetIAreoWndTransMode()
{
    if (!m_pIAreoWndTransMode)
        m_pIAreoWndTransMode = new IAreoWindowWrap(this);

    return m_pIAreoWndTransMode;
}
void*  AreoWindowWrap::QueryInterface(const IID* pIID)
{
    if (IsEqualIID(*pIID, uiiidof(IAreoWindowWrap)))  
    {   
        SetMsgHandled(TRUE);
        return (void*)GetIAreoWndTransMode();                 
    }                                              
    SetMsgHandled(FALSE);         
    return 0;
}

AREO_MODE  AreoWindowWrap::GetAeroMode()
{
    return m_eMode;
}   
void  AreoWindowWrap::GetBlurRegion(CRegion4* pregion)
{
    if (pregion && m_eMode == AREO_MODE_BLUR)
    {
        pregion->CopyRect(&blur.m_regionBlur);
    }
}

void  AreoWindowWrap::Enable(bool b)
{
    if (m_pDWM->IsEnable())
    {
        if (b)
        {
            if (m_pAreoDisableMode)
            {
                m_pAreoDisableMode->Enable(false);
                SAFE_RELEASE(m_pAreoDisableMode);
            }
            return;
            // 由外部调用UpdateRgn即可
            
        }
        else
        {
            if (m_eMode == AREO_MODE_BLUR)
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
            }
        }
    }
    
    do 
    {
        if (m_pAreoDisableMode)
        {
            m_pAreoDisableMode->Enable(true);
            break;
        }

        m_pAreoDisableMode = CreateTransparentModeByType(m_eAreoDisableMode);
        if (!m_pAreoDisableMode)
            break;

        m_pAreoDisableMode->Init(m_pWindow->m_pICustomWindow);
        m_pAreoDisableMode->Enable(true);

    } while (0);


}

void  AreoWindowWrap::UpdateRgn()
{
    if (m_pAreoDisableMode)
    {
        m_pAreoDisableMode->UpdateRgn();
        return;
    }


    DwmHelper*  pDwm = DwmHelper::GetInstance();
    if (pDwm->IsEnable())
    {
        if (m_eMode == AREO_MODE_BLUR)
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
        else if (AREO_MODE_TRANSPARENT == m_eMode)
        {
            if (m_pDWM->pDwmExtendFrameIntoClientArea)
            {
                m_pDWM->pDwmExtendFrameIntoClientArea(m_hWnd, &trans.m_margins);
            }
        }
    }
}

WINDOW_TRANSPARENT_MODE  AreoWindowWrap::GetModeValue()
{
    if (m_pAreoDisableMode)
        return m_pAreoDisableMode->GetModeValue();

    return WINDOW_TRANSPARENT_MODE_AREO;
}

bool  AreoWindowWrap::RequireAlphaChannel()
{
    if (m_pAreoDisableMode)
        return m_pAreoDisableMode->RequireAlphaChannel();

    return true;
}
bool  AreoWindowWrap::Commit()
{
    if (m_pAreoDisableMode)
    {
        return m_pAreoDisableMode->Commit();
    }

    return false;
}

}