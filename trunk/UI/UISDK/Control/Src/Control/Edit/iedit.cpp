#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\iedit.h"
#include "edit.h"
#include "instantedit.h"

using namespace UI;


UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IEdit, Edit, IControl)
UI_IMPLEMENT_Ixxx_INTERFACE(IInstantEdit, InstantEdit, Edit)

LPCTSTR  IEdit::GetText() 
{
	return m_pEditImpl->GetText(); 
}
void  IEdit::SetText(LPCTSTR szText, bool bUpdate) 
{
	m_pEditImpl->SetText(szText, bUpdate); 
}
void  IEdit::SetTextLong(long lNumber, bool bUpdate)
{
    TCHAR szText[16] = {0};
    _itot(lNumber, szText, 10);
    m_pEditImpl->SetText(szText, bUpdate); 
}
void  IEdit::SetTextNoFilter(LPCTSTR szText, bool bUpdate)
{
	m_pEditImpl->SetTextNoFilter(szText, bUpdate); 
}
void  IEdit::Clear(bool bUpdate) 
{
	m_pEditImpl->Clear(bUpdate);
}
void  IEdit::SetSel(int nStartChar, int nEndChar) 
{ 
	m_pEditImpl->SetSel(nStartChar, nEndChar); 
}
void  IEdit::GetSel(int& nStartChar,int& nEndChar) const 
{
	m_pEditImpl->GetSel(nStartChar, nEndChar); 
}
bool  IEdit::IsReadOnly()
{
    return m_pEditImpl->IsReadOnly();
}
void  IEdit::SetReadOnly(bool b, bool bUpdate)
{
    m_pEditImpl->SetReadOnly(b, bUpdate);
}
void  IEdit::SetTextFilterDigit()
{
	m_pEditImpl->SetTextFilterDigit();
}
void  IEdit::ClearTextFilter()
{
	m_pEditImpl->ClearTextFilter();
}


//////////////////////////////////////////////////////////////////////////

