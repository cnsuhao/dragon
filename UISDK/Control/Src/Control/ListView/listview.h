#pragma once
#include "UISDK\Control\Inc\Interface\ilistview.h"
#include "UISDK\Control\Inc\Interface\iheaderctrl.h"
#include "..\ScrollBar\vscrollbar_creator.h"
#include "..\ScrollBar\hscrollbar_creator.h"

namespace UI
{

class ListView;
class ListViewSubItemBase;

// 直接支持多列的列表控件
class  ListView : public MessageProxy
{
public:
	ListView();
	~ListView();

	UI_DECLARE_OBJECT(ListView, OBJ_CONTROL|CONTROL_LISTVIEWCTRL)

	UI_BEGIN_MSG_MAP	
		UIMSG_WM_SIZE(OnSize)
		UIMSG_WM_HSCROLL(OnHScroll)
        UIMSG_WM_MOUSEWHEEL(OnMouseWheel)
		UIMSG_HDN_TRACKING(OnHeaderCtrlTracking)
		UIMSG_HDN_TOTAL_WIDTH_CHANGED(OnHeaderCtrlTotalWidthChanged)
        UIMSG_HDN_ADDCOLUMN(OnAddColumn)
        UIMSG_HDN_REMOVECOLUMN(OnRemoveColumn)
        UIMSG_HDN_CLEARCOLUMN(OnClearColumn)
        UIMESSAGE_HANDLER_EX(UI_LVM_GETHEADERCTRL, OnGetHeaderCtrl)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
		UIMSG_WM_INITIALIZE(OnInitialize)
	UIALT_MSG_MAP(UIALT_CALLLESS)
		UIMSG_WM_CREATEBYEDITOR(OnCreateByEditor)
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ListView, IListCtrlBase)

public:
    void  SetIListView(IListView* p) { m_pIListView = p; SetIMessageProxy(static_cast<IMessage*>(p)); }
    IListView*  GetIListView() { return m_pIListView; }
    IHeaderListCtrl*  GetHeaderCtrl() { return m_pHeaderCtrl; }

    IListViewItemBase*  AddTextItem(LPCTSTR szText, int nAddFlag);
    IListViewItemBase*  InsertTextItem(int nIndex, LPCTSTR szText, int nInsertFlag);
    bool   SetItemText(int nItem, int nSubItem, LPCTSTR szText, bool bUpdate);
    
protected:
	void  OnSize( UINT nType, int cx, int cy );
	BOOL  OnHeaderCtrlTracking(int nColumnIndex, int* pNewWidth);
	void  OnHeaderCtrlTotalWidthChanged(int nTotalWidth);
    void  OnAddColumn(IListItemBase* pAddColumn);
    void  OnRemoveColumn(IListItemBase* pRemoveColumn);
    void  OnClearColumn();
	void  OnHScroll(int nSBCode, int nPos, IMessage* pMsgFrom);
    BOOL  OnMouseWheel(UINT nFlags, short zDelta, POINT pt);
    HRESULT  FinalConstruct(IUIApplication* p);
    LRESULT  OnGetHeaderCtrl(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void  OnCreateByEditor(CREATEBYEDITORDATA* pData);
	void  OnInitialize();

public:
	int    GetColumnCount();

protected:
	void   UpdateHeaderCtrlPos();

protected:
    IListView*  m_pIListView;
	IHeaderListCtrl*  m_pHeaderCtrl;

	VScrollbarCreator  m_vscrollbarCreator;
	HScrollbarCreator  m_hscrollbarCreator;
};
}