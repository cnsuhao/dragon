#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\icombobox.h"
#include "UISDK\Control\Src\Control\ComboBox\combobox.h"
#include "UISDK\Control\Src\Control\ListBase\PopupControlWindow\popupcontrolwindow.h"

using namespace UI;

UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IComboBoxBase, ComboBoxBase, IControl)
UI_IMPLEMENT_Ixxx_INTERFACE(IComboBox, ComboBox, ComboBoxBase)

LPCTSTR  IComboBoxBase::GetText()
{
	return m_pComboBoxBaseImpl->GetText(); 
}
void  IComboBoxBase::SetText(LPCTSTR szText, bool bUpdate)
{
	m_pComboBoxBaseImpl->SetText(szText, bUpdate);
}
IObject*  IComboBoxBase::GetContentObject(LPCTSTR szText)
{
	return m_pComboBoxBaseImpl->GetContentObject(szText);
}

void  IComboBoxBase::CloseUp() 
{ 
	m_pComboBoxBaseImpl->CloseUp(); 
}
IEdit*  IComboBoxBase::GetEdit() 
{ 
	return m_pComboBoxBaseImpl->GetEdit();
}

bool  IComboBox::AddString(LPCTSTR szText, int nAddItemFlags)
{
    return m_pComboBoxImpl->AddString(szText, nAddItemFlags);
}
IListBoxItem*  IComboBox::AddStringEx(LPCTSTR szText, int nAddItemFlags)
{
    return m_pComboBoxImpl->AddStringEx(szText, nAddItemFlags);
}
bool  IComboBox::SetCurSel(int nIndex)
{
    return m_pComboBoxImpl->SetCurSel(nIndex);
}
IListBox*  IComboBox::GetListBox()
{
    return m_pComboBoxImpl->GetListBox();
}
void  IComboBox::ResetContent()
{
	return m_pComboBoxImpl->ResetContent();
}

void  IComboBox::SetReadOnly(bool b)
{
    m_pComboBoxImpl->SetReadOnly(b);
}
bool  IComboBox::IsReadOnly()
{   
    return m_pComboBoxImpl->IsReadOnly();
}

IEdit*  IComboBox::GetEdit()
{
	return m_pComboBoxImpl->GetEdit();
}