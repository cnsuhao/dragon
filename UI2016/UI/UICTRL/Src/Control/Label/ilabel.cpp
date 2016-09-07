#include "stdafx.h"
#include "Inc\Interface\ilabel.h"
#include "Src\Control\Label\label.h"

namespace UI
{

UI_IMPLEMENT_INTERFACE_ACROSSMODULE(Label, Control);
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(PictureCtrl, Control);
UI_IMPLEMENT_INTERFACE_ACROSSMODULE(GifCtrl, Control);

LPCTSTR  ILabel::GetText()
{ 
	return __pImpl->GetText(); 
}
void  ILabel::SetText(LPCTSTR pszText, bool bRedraw) 
{
    __pImpl->SetText(pszText); 
    if (bRedraw)
        this->Invalidate();
}
IBindSourceChangedListener*  ILabel::GetListener()
{
	return __pImpl->GetListener();
}

bool  IGifCtrl::Start()
{ 
	return __pImpl->Start(); 
}
bool  IGifCtrl::Pause()
{
	return __pImpl->Pause();
}
bool  IGifCtrl::Stop() 
{ 
	return __pImpl->Stop();
}
}