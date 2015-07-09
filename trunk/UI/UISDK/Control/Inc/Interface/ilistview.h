#ifndef ILISTVIEW_H_2999C69E_54BD_473e_9DE6_06EA7AB660F6
#define ILISTVIEW_H_2999C69E_54BD_473e_9DE6_06EA7AB660F6
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"
#include "UISDK\Control\Inc\Interface\iheaderctrl.h"

namespace UI
{
interface IHeaderListCtrl;
#define LISTITEM_TYPE_LISTVIEWITEMBASE  136121301

// ����������ı�����
//  Wparam: UINT nIndex
//  LPARAM: LPCTSTR szText;
#define UI_LVIM_SETSUBITEMTEXT  136121750

// ��ȡ������ı�����
// WPARAM: UINT nIndex
// return: const TCUAR* 
#define UI_LVIM_GETSUBITEMTEXT  136121758

// wParam: nPos
#define UI_LVIM_ADDCOLUMN       136121751
// wParam: nPos
#define UI_LVIM_REMOVECOLUMN    136121752
#define UI_LVIM_CLEARCOLUMN     136121753




// ��ȡ����Ϣָ��
// return:  IHEADERCTRL*
#define UI_LVM_GETHEADERCTRL  136121304

class ListViewItemBase;
interface UICTRLAPI IListViewItemBase : public IListItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(IListViewItemBase, ListViewItemBase);
	bool  SetSubItemText(int nSub, LPCTSTR szText);
	LPCTSTR  GetSubItemText(int nSub);

};

class ListViewItemBaseShareData;
interface IListViewItemBaseShareData : public IListItemTypeShareData
{
    UI_DECLARE_Ixxx_INTERFACE(IListViewItemBaseShareData, ListViewItemBaseShareData);
};

class ListView;
interface UICTRLAPI IListView : public IListCtrlBase
{
    UI_DECLARE_Ixxx_INTERFACE(IListView, ListView);

    IHeaderListCtrl*  GetHeaderCtrl();

    IListViewItemBase*  AddTextItem(LPCTSTR szText, int nAddFlag);
    IListViewItemBase*  InsertTextItem(int nIndex, LPCTSTR szText, int nInsertFlag);
    bool   SetItemText(int nItem, int nSubItem, LPCTSTR szText, bool bUpdate);
};


}

#endif // ILISTVIEW_H_2999C69E_54BD_473e_9DE6_06EA7AB660F6