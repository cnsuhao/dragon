#ifndef IPROJECTTREEVIEW_598D78F0_1FED_4167_9204_AFEEDA7E1A0B
#define IPROJECTTREEVIEW_598D78F0_1FED_4167_9204_AFEEDA7E1A0B

#include "UISDK\Control\Inc\Interface\itreeview.h"

namespace UI
{
//
// ¿Ø¼þÍÏ×§ÊÂ¼þ  
// wParam: ProjectTreeViewControlItemDropData*
// lParam: NA.
// return: NA.
//    
#define UI_PROJECTTREEVIEW_NOTIFY_CONTROLITEM_DROPEVENT  1404141608
struct  ProjectTreeViewControlItemDropData
{
      DROPTARGETEVENT_TYPE   eType;
      DROPTARGETEVENT_DATA*  pData;
      IListItemBase*  pHitItem;
};



class SkinTreeViewItem;
interface UIEDITORCTRLAPI ISkinTreeViewItem : public INormalTreeItem
{
    UI_DECLARE_Ixxx_INTERFACE(ISkinTreeViewItem, SkinTreeViewItem)

public:
    void  SetTextRender(ITextRenderBase**  pTextRender);
};

class ControlTreeViewItem;
interface UIEDITORCTRLAPI IControlTreeViewItem : public INormalTreeItem
{
    UI_DECLARE_Ixxx_INTERFACE(IControlTreeViewItem, ControlTreeViewItem)
};



class ProjectTreeView;
interface UIEDITORCTRLAPI IProjectTreeView : public ITreeView
{
    UI_DECLARE_Ixxx_INTERFACE(IProjectTreeView, ProjectTreeView)

public:
    ISkinTreeViewItem*  InsertSkinItem(const TCHAR* szText, IListItemBase* pParent = UITVI_ROOT, IListItemBase* pInsertAfter = UITVI_LAST, int nInsertFlags=0);
    IControlTreeViewItem*  InsertControlItem(const TCHAR* szText, IListItemBase* pParent = UITVI_ROOT, IListItemBase* pInsertAfter = UITVI_LAST, int nInsertFlags=0);

};

}

#endif  // IPROJECTTREEVIEW_598D78F0_1FED_4167_9204_AFEEDA7E1A0B