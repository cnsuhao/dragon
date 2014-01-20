#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\iwndtransmode.h"
#include "UISDK\Kernel\Src\UIObject\Window\wndtransmode\layered\layeredwrap.h"
#include "UISDK\Kernel\Src\UIObject\Window\wndtransmode\aero\aerowrap.h"
#include "UISDK\Kernel\Src\UIObject\Window\wndtransmode\alpha\alphawrap.h"
#include "UISDK\Kernel\Src\UIObject\Window\wndtransmode\color\colorwrap.h"
#include "UISDK\Kernel\Src\UIObject\Window\wndtransmode\anti\antiwrap.h"

namespace UI
{
    
WINDOW_TRANSPARENT_MODE  GetTransparentModeTypeFromAttr(IMapAttribute* pMapAttrib)
{
    if (!pMapAttrib)
        return WINDOW_TRANSPARENT_MODE_UNKNOWN;

    const TCHAR* szText = pMapAttrib->GetAttr(XML_WINDOW_TRANSPARENT_TYPE, true);
    return GetTransparentModeTypeFromAttr(szText);
}

WINDOW_TRANSPARENT_MODE  GetTransparentModeTypeFromAttr(const TCHAR* szText)
{
    if (!szText)
        return WINDOW_TRANSPARENT_MODE_UNKNOWN;

    if (_T('\0') == szText[0])
        return WINDOW_TRANSPARENT_MODE_NORMAL;

    if (0 == _tcscmp(XML_WINDOW_TRANSPARENT_TYPE_AERO, szText))
    {
        return WINDOW_TRANSPARENT_MODE_AERO;
    }
    else if (0 == _tcscmp(XML_WINDOW_TRANSPARENT_TYPE_LAYERED, szText))
    {
        return WINDOW_TRANSPARENT_MODE_LAYERED;
    }
    else if (0 == _tcscmp(XML_WINDOW_TRANSPARENT_TYPE_MASKCOLOR, szText))
    {	
        return WINDOW_TRANSPARENT_MODE_MASKCOLOR;
    }	
    else if (0 == _tcscmp(XML_WINDOW_TRANSPARENT_TYPE_MASKALPHA, szText))
    {
        return WINDOW_TRANSPARENT_MODE_MASKALPHA;
    }
    else if (0 == _tcscmp(XML_WINDOW_TRANSPARENT_TYPE_ANTIALIASING, szText))
    {
        return WINDOW_TRANSPARENT_MODE_ANTIALIASING;
    }

    return WINDOW_TRANSPARENT_MODE_UNKNOWN;
}

IWndTransMode*  CreateTransparentModeByType(WINDOW_TRANSPARENT_MODE eType)
{
    switch (eType)
    {
    case WINDOW_TRANSPARENT_MODE_AERO:
        {
            return static_cast<IWndTransMode*>(new AeroWindowWrap());
        }
        break;

    case WINDOW_TRANSPARENT_MODE_LAYERED:
        {
            return static_cast<IWndTransMode*>(new LayeredWindowWrap());
        }
        break;

    case WINDOW_TRANSPARENT_MODE_ANTIALIASING:
        {
            return static_cast<IWndTransMode*>(new AntiWindowWrap());
        }
        break;;

    case WINDOW_TRANSPARENT_MODE_MASKALPHA:
        {
            return static_cast<IWndTransMode*>(new AlphaMaskWindowWrap());
        }
        break;;

    case WINDOW_TRANSPARENT_MODE_MASKCOLOR:
        {
            return static_cast<IWndTransMode*>(new ColorMaskWindowWrap());
        }
        break;

    default:
        return NULL;
    }

    return NULL;
}


IAeroWindowWrap::IAeroWindowWrap(AeroWindowWrap*  p)
{
    m_pImpl = p;
}

AERO_MODE  IAeroWindowWrap::GetAeroMode()
{
    return m_pImpl->GetAeroMode();
}
void  IAeroWindowWrap::GetBlurRegion(CRegion4* pregion)
{
    m_pImpl->GetBlurRegion(pregion);
}
void  IAeroWindowWrap::UpdateRgn()
{
    m_pImpl->UpdateRgn();
}


//////////////////////////////////////////////////////////////////////////


ILayeredWindowWrap::ILayeredWindowWrap(LayeredWindowWrap*  p)
{
    m_pImpl = p;
}
byte  ILayeredWindowWrap::GetAlpha()
{
    return m_pImpl->GetAlpha();
}
void  ILayeredWindowWrap::SetAlpha(byte b, bool bUpdate)
{
    m_pImpl->SetAlpha(b, bUpdate);
}

}