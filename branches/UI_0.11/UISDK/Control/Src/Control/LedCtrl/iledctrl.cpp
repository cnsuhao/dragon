#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\iledctrl.h"
#include "UISDK\Control\Src\Control\LedCtrl\ledctrl.h"
#include "UISDK\Control\Src\Control\LedCtrl\ledexctrl.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(ILEDCtrl, LEDCtrl, IControl)
UI_IMPLEMENT_Ixxx_INTERFACE(ILEDExCtrl, LEDExCtrl, ButtonBase)

void  ILEDCtrl::SetIndexMap(LPCTSTR szText) { return m_pLEDCtrlImpl->SetIndexMap(szText); }
void  ILEDCtrl::SetText(LPCTSTR szText, bool bUpdate) { return m_pLEDCtrlImpl->SetText(szText, bUpdate); }

void  ILEDExCtrl::SetItemInfo(LEDEX_ITEM_INFO* pInfoArray, int nCount)
{
    return m_pLEDExCtrlImpl->SetItemInfo(pInfoArray, nCount);
}
void  ILEDExCtrl::SetText(LPCTSTR szText, bool bUpdate) 
{
    return m_pLEDExCtrlImpl->SetText(szText, bUpdate); 
}

}