#pragma once
#include "UISDK\Project\UIEditorCtrl\Inc\iprojecttreeview.h"

namespace UI
{

class ProjectTreeView : public MessageProxy
{
public:
    ProjectTreeView();
    ~ProjectTreeView();

    UI_DECLARE_OBJECT(ProjectTreeView, OBJ_CONTROL|CONTROL_TREEVIEWCTRL)

    UI_BEGIN_MSG_MAP
        UIMSG_WM_DROPTARGETEVENT(OnDropTargetEvent)
        UIMSG_WM_NOTIFY_CODE_FROM(UI_LCN_DRAWITEM, m_pIProjectTreeView, OnDrawItem)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ProjectTreeView, ITreeView)

    void  SetIProjectTreeView(IProjectTreeView* p)
    {
        m_pIProjectTreeView = p;
        SetIMessageProxy(static_cast<IMessage*>(p));
    }

public:
    void  OnInitialize();
    void  SetAttribute(IMapAttribute* pMapAttr, bool bReload);
    void  OnDropTargetEvent(DROPTARGETEVENT_TYPE eType, DROPTARGETEVENT_DATA* pData);
    LRESULT  OnDrawItem(WPARAM w, LPARAM l);

public:
    ISkinTreeViewItem*  InsertSkinItem(const TCHAR* szText, IListItemBase* pParent = UITVI_ROOT, IListItemBase* pInsertAfter = UITVI_LAST, int nInsertFlags=0);
    IControlTreeViewItem*  InsertControlItem(const TCHAR* szText, IListItemBase* pParent = UITVI_ROOT, IListItemBase* pInsertAfter = UITVI_LAST, int nInsertFlags=0);

protected:
    IProjectTreeView*   m_pIProjectTreeView;
    ITextRenderBase*    m_pSkinItemText;
    IListItemBase*      m_pDragDropHitItem;  // 拖拽过程中，鼠标下的item
//    POINT               m_ptItemDragDropHit; // 拖拽过程中，鼠标在item中的位置
};

}