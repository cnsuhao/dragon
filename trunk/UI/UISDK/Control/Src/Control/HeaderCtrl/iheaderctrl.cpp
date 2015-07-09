#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\iheaderctrl.h"
#include "UISDK\Control\Src\Control\HeaderCtrl\headerctrl.h"
#include "UISDK\Control\Src\Control\HeaderCtrl\headerlistctrl.h"
#include "UISDK\Control\Src\Control\HeaderCtrl\headerlistitem.h"

using namespace UI;

UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IHeaderListCtrl, HeaderListCtrl, IListCtrlBase);
UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IHeaderListItem, HeaderListItem, IListItemBase);

IHeaderListItem*  IHeaderListCtrl::AddColumn(LPCTSTR szText, int nWidth, int nFormat) 
{
    return m_pHeaderListCtrlImpl->AddColumn(szText, nWidth, nFormat); 
}
IHeaderListItem*  IHeaderListCtrl::InsertColumn(int nPos, LPCTSTR szText, int nWidth, int nFormat) 
{
    return m_pHeaderListCtrlImpl->InsertColumn(nPos, szText, nWidth, nFormat); 
}

void  IHeaderListCtrl::SetBindControl(IMessage* p)
{
    return m_pHeaderListCtrlImpl->SetBindControl(p);
}
IMessage*  IHeaderListCtrl::GetBindControl()
{
    return m_pHeaderListCtrlImpl->GetBindControl();
}

int   IHeaderListCtrl::GetColumnCount()                       
{
    return m_pHeaderListCtrlImpl->GetColumnCount(); 
}
bool  IHeaderListCtrl::CanDragColumn()                        
{ 
    return m_pHeaderListCtrlImpl->CanDragColumn(); 
}
void  IHeaderListCtrl::SetHScrollOffset(int nhScrollOffset)   
{ 
    m_pHeaderListCtrlImpl->SetHScrollOffset(nhScrollOffset); 
}
int   IHeaderListCtrl::GetHScrollOffset()                     
{ 
    return m_pHeaderListCtrlImpl->GetHScrollOffset(); 
}
int   IHeaderListCtrl::GetColumnTotalWidth()                 
{ 
    return m_pHeaderListCtrlImpl->GetColumnTotalWidth(); 
}

void  IHeaderListItem::SetTextAlign(int n)                    
{
    m_pHeaderListItemImpl->SetTextAlign(n); 
}
int   IHeaderListItem::GetTextAlign()                         
{
    return m_pHeaderListItemImpl->GetTextAlign(); 
}

void  IHeaderListItem::GetColumnInfo(ColumnInfo* pInfo)
{
    m_pHeaderListItemImpl->GetColumnInfo(pInfo);
}
void  IHeaderListItem::SetColumnInfo(ColumnInfo* pInfo)
{
    m_pHeaderListItemImpl->SetColumnInfo(pInfo);
}