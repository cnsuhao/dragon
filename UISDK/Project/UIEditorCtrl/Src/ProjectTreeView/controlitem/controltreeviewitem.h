#pragma once
#include "UISDK\Project\UIEditorCtrl\Inc\iprojecttreeview.h"

namespace UI
{
class ControlTreeViewItem : public MessageProxy
{
public:
    ControlTreeViewItem();
    ~ControlTreeViewItem();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
        UIMSG_WM_MOUSEMOVE(OnMouseMove)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ControlTreeViewItem, INormalTreeItem)

    void  SetIControlTreeViewItem(IControlTreeViewItem* p)
    {
        m_pIControlTreeViewItem = p; 
        SetIMessageProxy(static_cast<IMessage*>(p));
    }

public:
    void  OnMouseMove(UINT nFlags, POINT point);
    void  OnLButtonDown(UINT nFlags, POINT point);
    void  DoDrag(POINT point);
    HBITMAP  CreateDragBitmap(int* pWidth, int* pHeight);

public:
    IControlTreeViewItem*  m_pIControlTreeViewItem;
    POINT  m_ptLastLButtonDown;
    static UINT  s_nControlDragCF;
};
}