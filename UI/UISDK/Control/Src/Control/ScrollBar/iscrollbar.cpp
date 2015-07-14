#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\iscrollbar.h"
#include "UISDK\Control\Src\Control\ScrollBar\systemscrollbar.h"

using namespace UI;

UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(ISystemScrollBar, SystemScrollBar, IControl)
UI_IMPLEMENT_Ixxx_INTERFACE(ISystemHScrollBar, SystemHScrollBar, SystemScrollBar)
UI_IMPLEMENT_Ixxx_INTERFACE(ISystemVScrollBar, SystemVScrollBar, SystemScrollBar)

void  ISystemScrollBar::SetIScrollBarMgr(IScrollBarManager* p)
{
    m_pSystemScrollBarImpl->SetIScrollBarMgr(p); 
}
void  ISystemScrollBar::SetAutoSetBindObjFocus(bool b)
{
    m_pSystemScrollBarImpl->SetAutoSetBindObjFocus(b);
}

void  ISystemScrollBar::SetFloatOnClientRegion(bool b)
{
    m_pSystemScrollBarImpl->SetFloatOnClientRegion(b);
}
void  ISystemScrollBar::SetKeepHoldNonClientRegion(bool b)
{
    m_pSystemScrollBarImpl->SetKeepHoldNonClientRegion(b);
}