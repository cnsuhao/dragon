#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\iflashwrap.h"
#include "UISDK\Control\Src\Control\FlashWrap\flashwrap.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IFlashWrap, FlashWrap, IControl);

void  IFlashWrap::Pause()
{ 
	m_pFlashWrapImpl->Pause();
}
void  IFlashWrap::Play() 
{ 
	m_pFlashWrapImpl->Play();
}
void  IFlashWrap::SetFlashUri(LPCTSTR szUri)
{ 
	m_pFlashWrapImpl->SetFlashUri(szUri);
}
HRESULT  IFlashWrap::CallFlashFunction(LPCTSTR szFuncName, LPCTSTR szArgs, BSTR* pbstrRet) 
{
    return m_pFlashWrapImpl->CallFlashFunction(szFuncName, szArgs, pbstrRet);
}

}