#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\ilabel.h"
#include "UISDK\Control\Src\Control\Label\label.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(ILabel, Label, IControl);
UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IPictureCtrl, PictureCtrl, IControl);
UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IGifCtrl, GifCtrl, IControl);

LPCTSTR  ILabel::GetText() { return m_pLabelImpl->GetText(); }
void  ILabel::SetText(LPCTSTR pszText, bool bRedraw) 
{
    m_pLabelImpl->SetText(pszText); 
    if (bRedraw)
        this->UpdateObject();
}


bool  IGifCtrl::Start() { return m_pGifCtrlImpl->Start(); }
bool  IGifCtrl::Pause() { return m_pGifCtrlImpl->Pause(); }
bool  IGifCtrl::Stop()  { return m_pGifCtrlImpl->Stop(); }
}