#include "stdafx.h"
#include "propertyctrl.h"
#include "item\propertyctrlitembase.h"
#include "item\propertyctrlgroupitem.h"
#include "item\propertyctrledititem.h"
#include "item\propertyctrlcomboboxitem.h"
#include "item\propertyctrlbuttonitem.h"
#include "item\propertyctrllongitem.h"
#include "item\propertyctrlboolitem.h"
#include "item\propertyctrladditionitem.h"
#include "UISDK\Project\UIEditorCtrl\Inc\ipropertyctrl.h"

using namespace UI;


UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IPropertyCtrl, PropertyCtrl, ITreeView);
UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IPropertyCtrlGroupItem, PropertyCtrlGroupItem, INormalTreeItem);

UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IPropertyCtrlItemBase, PropertyCtrlItemBase, IListItemBase);
UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IPropertyCtrlItemBaseShareData, PropertyCtrlItemBaseShareData, IListItemTypeShareData);

UI_IMPLEMENT_Ixxx_INTERFACE(IPropertyCtrlEditItem, PropertyCtrlEditItem, PropertyCtrlItemBase);
UI_IMPLEMENT_Ixxx_INTERFACE(IPropertyCtrlAdditionItem, PropertyCtrlAdditionItem, PropertyCtrlItemBase);
UI_IMPLEMENT_Ixxx_INTERFACE(IPropertyCtrlComboBoxItem, PropertyCtrlComboBoxItem, PropertyCtrlItemBase);
UI_IMPLEMENT_Ixxx_INTERFACE(IPropertyCtrlBoolItem, PropertyCtrlBoolItem, PropertyCtrlComboBoxItem);
UI_IMPLEMENT_Ixxx_INTERFACE(IPropertyCtrlLongItem, PropertyCtrlLongItem, PropertyCtrlEditItem);
UI_IMPLEMENT_Ixxx_INTERFACE(IPropertyCtrlButtonItem, PropertyCtrlButtonItem, PropertyCtrlItemBase);

IPropertyCtrlGroupItem*  IPropertyCtrl::InsertGroupItem(
        LPCTSTR szName, LPCTSTR szDesc, 
        IListItemBase* pParent, IListItemBase* pInsertAfter, LISTITEM_OPFLAGS nInsertFlags)
{
    return m_pPropertyCtrlImpl->InsertGroupItem(szName, szDesc, pParent, pInsertAfter, nInsertFlags);
}

IPropertyCtrlGroupItem*  IPropertyCtrl::FindGroupItem(IListItemBase* pParentItem, LPCTSTR szName)
{
    return m_pPropertyCtrlImpl->FindGroupItem(pParentItem, szName);
}
IListItemBase*  IPropertyCtrl::FindItemByKey(LPCTSTR szKey)
{
    return m_pPropertyCtrlImpl->FindItemByKey(szKey);
}

IPropertyCtrlEditItem*   IPropertyCtrl::InsertEditProperty(
        PropertyCtrlEditItemInfo* pInfo,
        IListItemBase* pParentItem, IListItemBase* pInsertAfter, LISTITEM_OPFLAGS nInsertFlags)
{
    return m_pPropertyCtrlImpl->InsertEditProperty(pInfo, pParentItem, pInsertAfter, nInsertFlags);
}

IPropertyCtrlLongItem*   IPropertyCtrl::InsertLongProperty(
        PropertyCtrlLongItemInfo* pInfo, 
        IListItemBase* pParentItem, IListItemBase* pInsertAfter, LISTITEM_OPFLAGS nInsertFlags)
{
    return m_pPropertyCtrlImpl->InsertLongProperty(pInfo, pParentItem, pInsertAfter, nInsertFlags);
}

IPropertyCtrlBoolItem*   IPropertyCtrl::InsertBoolProperty(
        PropertyCtrlBoolItemInfo* pInfo,
		IListItemBase* pParentItem, IListItemBase* pInsertAfter, LISTITEM_OPFLAGS nInsertFlags)
{
	return m_pPropertyCtrlImpl->InsertBoolProperty(pInfo, pParentItem, pInsertAfter, nInsertFlags);
}

IPropertyCtrlComboBoxItem*   IPropertyCtrl::InsertComboBoxProperty(
        PropertyCtrlComboBoxItemInfo* pInfo,
		IListItemBase* pParentItem, IListItemBase* pInsertAfter, LISTITEM_OPFLAGS nInsertFlags)
{
	return m_pPropertyCtrlImpl->InsertComboBoxProperty(pInfo, pParentItem, pInsertAfter, nInsertFlags);
}

IPropertyCtrlButtonItem*   IPropertyCtrl::InsertButtonProperty(
        LPCTSTR szText, LPCTSTR szValue, LPCTSTR szDesc, LPCTSTR szKey,
		IListItemBase* pParentItem, IListItemBase* pInsertAfter, LISTITEM_OPFLAGS nInsertFlags)
{
	return m_pPropertyCtrlImpl->InsertButtonProperty(szText, szValue, szDesc, szKey, pParentItem, pInsertAfter, nInsertFlags);
}

IPropertyCtrlAdditionItem*  IPropertyCtrl::AddAdditionItem(LISTITEM_OPFLAGS nFlags)
{
	return m_pPropertyCtrlImpl->AddAdditionItem(nFlags);
}

IPropertyCtrlEditItem*  IPropertyCtrl::FindEditItem(LPCTSTR szKey)
{
	return m_pPropertyCtrlImpl->FindEditItem(szKey);
}
IPropertyCtrlLongItem*  IPropertyCtrl::FindLongItem(LPCTSTR szKey)
{
    return m_pPropertyCtrlImpl->FindLongItem(szKey);
}
IPropertyCtrlComboBoxItem*  IPropertyCtrl::FindComboBoxItem(LPCTSTR szKey)
{
    return m_pPropertyCtrlImpl->FindComboBoxItem(szKey);
}
IPropertyCtrlBoolItem*  IPropertyCtrl::FindBoolItem(LPCTSTR szKey)
{
	return m_pPropertyCtrlImpl->FindBoolItem(szKey);
}
//////////////////////////////////////////////////////////////////////////

LPCTSTR  IPropertyCtrlItemBase::GetKey()
{
    return m_pPropertyCtrlItemBaseImpl->GetKey();
}

void  IPropertyCtrlEditItem::SetValueText(LPCTSTR szText)   
{
    m_pPropertyCtrlEditItemImpl->SetValueText(szText); 
}
void  IPropertyCtrlEditItem::SetDefaultValueText(LPCTSTR szText) 
{
    m_pPropertyCtrlEditItemImpl->SetDefaultValueText(szText); 
}

void  IPropertyCtrlComboBoxItem::AddOption(LPCTSTR szText, long lValue) 
{
    m_pPropertyCtrlComboBoxItemImpl->AddOption(szText, lValue); 
}
void  IPropertyCtrlComboBoxItem::SetValueString(LPCTSTR szText)
{
	m_pPropertyCtrlComboBoxItemImpl->SetValueString(szText); 
}
void  IPropertyCtrlComboBoxItem::SetValueLong(long lValue)
{
	m_pPropertyCtrlComboBoxItemImpl->SetValueLong(lValue); 
}

void  IPropertyCtrlButtonItem::SetValueText(LPCTSTR szText)   
{
    m_pPropertyCtrlButtonItemImpl->SetValueText(szText); 
}
void  IPropertyCtrlButtonItem::SetDefaultValueText(LPCTSTR szText) 
{
    m_pPropertyCtrlButtonItemImpl->SetDefaultValueText(szText); 
}

void  IPropertyCtrlLongItem::SetValue(long lValue)
{
	m_pPropertyCtrlLongItemImpl->SetValue(lValue);
}
void  IPropertyCtrlBoolItem::SetBool(bool b)
{
	m_pPropertyCtrlBoolItemImpl->SetBool(b);
}