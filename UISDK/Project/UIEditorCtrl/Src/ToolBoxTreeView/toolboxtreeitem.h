#pragma once
#include "UISDK\Project\UIEditorCtrl\Inc\itoolboxtreeview.h"

namespace UI
{

class ToolBoxTreeItem : public MessageProxy
{
public:
    ToolBoxTreeItem();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
        UIMSG_WM_LBUTTONUP(OnLButtonUp)
        UIMSG_WM_MOUSEMOVE(OnMouseMove)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ToolBoxTreeItem, INormalTreeItem)

    void  SetIToolBoxTreeItem(IToolBoxTreeItem* p)
    {
        m_pIToolBoxTreeItem = p;
        SetIMessageProxy(static_cast<IToolBoxTreeItem*>(m_pIToolBoxTreeItem));
    }

    void  OnMouseMove(UINT nFlags, POINT point);
    void  OnLButtonDown(UINT nFlags, POINT point);
    void  OnLButtonUp(UINT nFlags, POINT point);

protected:
    void  DoDrag(POINT pt);
    HBITMAP  CreateDragBitmap(int* pWidth, int* pHeight);

private:
    IToolBoxTreeItem*  m_pIToolBoxTreeItem;
    POINT  m_ptLastLButtonDown;

public:
    static UINT   s_nLayoutItemDragCF;  // ÍÏ×§¸ñÊ½
};


}