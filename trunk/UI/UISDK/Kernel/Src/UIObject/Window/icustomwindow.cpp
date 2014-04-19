#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
#include "UISDK\Kernel\Src\UIObject\Window\customwindow.h"

namespace UI
{
UI_IMPLEMENT_Ixxx_INTERFACE(ICustomWindow, CustomWindow, Window)

void  ICustomWindow::SetWindowResizeType(UINT nType)           { m_pCustomWindowImpl->SetResizeCapability(nType); }
void  ICustomWindow::SetWndTransMode(WINDOW_TRANSPARENT_MODE eMode, bool bRedraw) { m_pCustomWindowImpl->SetWndTransMode(eMode, bRedraw); }
WINDOW_TRANSPARENT_MODE  ICustomWindow::GetWndTransMode()      { return m_pCustomWindowImpl->GetWndTransMode(); }
IWndTransMode*  ICustomWindow::GetWndTransModePtr()      { return m_pCustomWindowImpl->GetWndTransModePtr(); }
}