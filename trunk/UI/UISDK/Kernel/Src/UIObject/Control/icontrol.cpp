#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\icontrol.h"
#include "UISDK\Kernel\Src\UIObject\Control\control.h"

using namespace UI;

UI_IMPLEMENT_Ixxx_INTERFACE(IControl, Control, Object)

void  IControl::ModifyControlStyle(ControlStyle* add, ControlStyle* remove)
{
	m_pControlImpl->ModifyControlStyle(add, remove);
}
bool  IControl::TestControlStyle(ControlStyle* test)
{
	return m_pControlImpl->TestControlStyle(test);
}
bool  IControl::IsGroup()
{ 
	return m_pControlImpl->IsGroup();
}
void  IControl::SetGroup(bool b)     
{ 
	return m_pControlImpl->SetGroup(b); 
}
ITextRenderBase*  IControl::CreateDefaultTextRender()
{
    return m_pControlImpl->CreateDefaultTextRender();
}
