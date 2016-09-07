#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
#include "UISDK\Kernel\Src\UIObject\Window\customwindow.h"

namespace UI
{
UI_IMPLEMENT_Ixxx_INTERFACE(ICustomWindow, CustomWindow, Window)

void  ICustomWindow::SetWindowResizeType(UINT nType)         
{ 
    m_pCustomWindowImpl->SetResizeCapability(nType); 
}
void  ICustomWindow::SetWindowTransparentType(WINDOW_TRANSPARENT_TYPE eMode) 
{
    m_pCustomWindowImpl->SetWindowTransparentType(eMode); 
}
WINDOW_TRANSPARENT_TYPE  ICustomWindow::GetWindowTransparentType()    
{
    return m_pCustomWindowImpl->GetWindowTransparentType(); 
}
IWindowTransparent*  ICustomWindow::GetWindowTransparent()   
{ 
    return m_pCustomWindowImpl->GetWindowTransparentPtr();
}
}