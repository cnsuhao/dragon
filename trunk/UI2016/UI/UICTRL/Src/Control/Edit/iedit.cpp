#include "stdafx.h"
#include "edit.h"
#include "Inc\Interface\iedit.h"
#include "instantedit.h"
#include "password_edit.h"

UI_IMPLEMENT_INTERFACE_ACROSSMODULE(Edit, Control)
UI_IMPLEMENT_INTERFACE(InstantEdit, Edit)
UI_IMPLEMENT_INTERFACE(PasswordEdit, Edit)

LPCTSTR  IEdit::GetText() 
{
	return __pImpl->GetText(); 
}
void  IEdit::SetText(LPCTSTR szText) 
{
	__pImpl->SetText(szText); 
}
void  IEdit::SetTextLong(long lNumber)
{
    TCHAR szText[16] = {0};
    _itot(lNumber, szText, 10);
    __pImpl->SetText(szText); 
}
void  IEdit::SetTextNoFilter(LPCTSTR szText)
{
	__pImpl->SetTextNoFilter(szText); 
}
void  IEdit::Clear() 
{
	__pImpl->Clear();
}
void  IEdit::SetSel(int nStartChar, int nEndChar) 
{ 
	__pImpl->SetSel(nStartChar, nEndChar); 
}
void  IEdit::GetSel(int& nStartChar,int& nEndChar) const 
{
	__pImpl->GetSel(nStartChar, nEndChar); 
}
bool  IEdit::IsReadOnly()
{
    return __pImpl->IsReadOnly();
}
void  IEdit::SetReadOnly(bool b)
{
    __pImpl->SetReadOnly(b);
}
void  IEdit::SetWantTab(bool b)
{
    __pImpl->SetWantTab(b);
}
void  IEdit::SetTextFilterDigit()
{
	__pImpl->SetTextFilterDigit();
}
void  IEdit::ClearTextFilter()
{
	__pImpl->ClearTextFilter();
}

signal<IEdit*, bool>&  IEdit::EnChangeEvent()
{
    return __pImpl->en_change;
}

signal_r<bool, IEdit*, UINT>&  IEdit::KeyDownEvent()
{
    return __pImpl->keydown;
}
