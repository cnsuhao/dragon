#pragma once
#include "UISDK\Control\Inc\Interface\itreeview.h"
#include "UISDK\Control\Src\Control\TreeView\treeitem.h"
#include "UISDK\Control\Src\Control\ScrollBar\vscrollbar_creator.h"
#include "UISDK\Control\Src\Control\ScrollBar\hscrollbar_creator.h"

namespace UI
{
class TreeView : public MessageProxy
{
public:
    TreeView();
    ~TreeView();
    UI_DECLARE_OBJECT(TreeView, OBJ_CONTROL|CONTROL_TREEVIEWCTRL)

    UI_BEGIN_MSG_MAP
        UIMSG_WM_KEYDOWN(OnKeyDown)
        UIMESSAGE_HANDLER_EX(UI_TVM_GETEXPANDICON_RENDER, 
            OnGetExpandIconRender)
        UIMSG_WM_SERIALIZE(OnSerialize)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
		UIMSG_WM_INITIALIZE(OnInitialize)
	UIALT_MSG_MAP(UIALT_CALLLESS)
		UIMSG_WM_CREATEBYEDITOR(OnCreateByEditor)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(TreeView, IListCtrlBase)

public:
    void  SetITreeView(ITreeView* p) { 
            m_pITreeView = p; 
            SetIMessageProxy(static_cast<IMessage*>(p)); 
    }
    
    ITreeItem*  InsertNormalItem(
            UITVITEM* pItem, 
            IListItemBase* pParent = UITVI_ROOT, 
            IListItemBase* pInsertAfter = UITVI_LAST, 
            int nInsertFlags=0);
    ITreeItem*  InsertNormalItem(
            LPCTSTR szText, 
            IListItemBase* pParent = UITVI_ROOT, 
            IListItemBase* pInsertAfter = UITVI_LAST, 
            int nInsertFlags=0);

    IRenderBase*  GetExpandIconRender() { return m_pExpandIcon; }

protected:
    HRESULT  FinalConstruct(IUIApplication* p);
    void  OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void  OnSerialize(SERIALIZEDATA* pData);
	void  OnInitialize();
	void  OnCreateByEditor(CREATEBYEDITORDATA* pData);

    LRESULT  OnGetExpandIconRender(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LPCTSTR  SaveExpandIconRender() ;
	void  LoadExpandIconRender(LPCTSTR szName) ;

protected:
    ITreeView*  m_pITreeView;
    IRenderBase*   m_pExpandIcon;    // 展开/收缩图标绘制

	VScrollbarCreator  m_vscrollbarCreator;
	HScrollbarCreator  m_hscrollbarCreator;
};
}