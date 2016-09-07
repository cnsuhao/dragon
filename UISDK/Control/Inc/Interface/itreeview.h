#ifndef ITREEVIEW_H_5932341A_E38D_425f_BC91_F0E765F1CA93
#define ITREEVIEW_H_5932341A_E38D_425f_BC91_F0E765F1CA93

#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"

namespace UI
{
#define LISTITEM_TYPE_TREEITEM           136121826
#define LISTITEM_TYPE_TREEITEM_COMPOUND  136121827
#define LISTITEM_TYPE_TREEITEM_NORMAL    136121828

#define UITVIF_TEXT   TVIF_TEXT	
#define UITVIF_IMAG   TVIF_IMAGE
#define UITVIF_PARAM  TVIF_PARAM

// 获取展开/收缩图标renderbase
// WPARAM:  IRenderBase**
#define UI_TVM_GETEXPANDICON_RENDER  136131631


#define TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_EXPAND_NORMAL    (RENDER_STATE_NORMAL|0)
#define TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_EXPAND_HOVER     (RENDER_STATE_HOVER|1)
#define TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_EXPAND_PRESS     (RENDER_STATE_PRESS|2);
#define TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_EXPAND_DISABLE   (RENDER_STATE_DISABLE|3)
#define TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_COLLAPSE_NORMAL  (RENDER_STATE_SELECTED|RENDER_STATE_NORMAL|4)
#define TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_COLLAPSE_HOVER   (RENDER_STATE_SELECTED|RENDER_STATE_HOVER|5)
#define TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_COLLAPSE_PRESS   (RENDER_STATE_SELECTED|RENDER_STATE_PRESS|6)
#define TREEVIEWCTRL_EXPANDCOLLAPSEICON_RENDER_STATE_COLLAPSE_DISABLE (RENDER_STATE_SELECTED|RENDER_STATE_DISABLE|7)


struct UITVITEM
{
    UINT         mask;
    TCHAR*       pszText;
    int          cchTextMax;   // Set时忽略
    IRenderBase*  pIconRender;  
    LPARAM       pData;
};


class TreeItem;
interface UICTRLAPI ITreeItem : public IListItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(ITreeItem, TreeItem);      
};


class TreeView;
interface UICTRLAPI ITreeView : public IListCtrlBase
{
    UI_DECLARE_Ixxx_INTERFACE(ITreeView, TreeView);
    
    IRenderBase*  GetExpandIconRender();

    ITreeItem*  InsertNormalItem(UITVITEM* pItem, IListItemBase* pParent = UITVI_ROOT, IListItemBase* pInsertAfter = UITVI_LAST, int nInsertFlags=0);
    ITreeItem*  InsertNormalItem(LPCTSTR szText, IListItemBase* pParent = UITVI_ROOT, IListItemBase* pInsertAfter = UITVI_LAST, int nInsertFlags=0);

};

//////////////////////////////////////////////////////////////////////////

class NormalTreeItemShareData;
interface UICTRLAPI INormalTreeItemShareData : public IListItemTypeShareData
{
    UI_DECLARE_Ixxx_INTERFACE(INormalTreeItemShareData, NormalTreeItemShareData);
    int  GetImageWidth();
    int  GetExpandIconWidth();
	IRenderBase*  GetExpandIconRender();
};

class NormalTreeItem;
interface UICTRLAPI INormalTreeItem : public ITreeItem
{
    UI_DECLARE_Ixxx_INTERFACE(INormalTreeItem, NormalTreeItem);      
};


}

#endif // ITREEVIEW_H_5932341A_E38D_425f_BC91_F0E765F1CA93