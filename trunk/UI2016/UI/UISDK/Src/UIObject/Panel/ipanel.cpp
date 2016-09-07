#include "stdafx.h"
#include "Inc\Interface\ipanel.h"
#include "Src\UIObject\Panel\panel.h"

namespace UI
{
UI_IMPLEMENT_INTERFACE(Panel, Object)

ILayout* IPanel::GetLayout()                 
{
    return __pImpl->GetLayout(); 
}
void  IPanel::SetLayoutType(LAYOUTTYPE eLayoutType)  
{ 
    __pImpl->SetLayoutType(eLayoutType);
}
void  IPanel::SetTextureRender(IRenderBase* p)
{
    __pImpl->SetTextureRender(p);
}
IRenderBase*  IPanel::GetTextureRender()     
{ 
    return __pImpl->GetTextureRender();
}
}