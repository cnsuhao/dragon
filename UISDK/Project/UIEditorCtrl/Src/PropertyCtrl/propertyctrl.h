#pragma once
#include "UISDK\Project\UIEditorCtrl\Inc\ipropertyctrl.h"

namespace UI
{

interface IEdit;
interface IButton;
interface IComboBox;

class PropertyCtrl : public MessageProxy
{
public:
    PropertyCtrl();
    ~PropertyCtrl();

    UI_DECLARE_OBJECT(PropertyCtrl, OBJ_CONTROL|CONTROL_TREEVIEWCTRL)

    UI_BEGIN_MSG_MAP
        UIMSG_WM_SIZE(OnSize)
        UIMESSAGE_HANDLER_EX(UI_PROPERTYCTRL_MSG_GETSPLITTERLINEPOS_PTR, OnGetSplitterLinePosPtr)
        UIMESSAGE_HANDLER_EX(UI_WM_SHOW_TOOLTIP, OnPreShowTooltip)
        UIMESSAGE_HANDLER_EX(UI_PROPERTYCTRL_MSG_GETEDITCTRL, OnGetEditCtrl)
		UIMESSAGE_HANDLER_EX(UI_PROPERTYCTRL_MSG_GETCOMBOBOXCTRL, OnGetComboBoxCtrl)
		UIMESSAGE_HANDLER_EX(UI_PROPERTYCTRL_MSG_GETBUTTONCTRL, OnGetButtonCtrl)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
		UIMSG_WM_INITIALIZE(OnInitialize)
        UIMSG_WM_INITIALIZE2(OnInitialize2)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
        UIMSG_WM_FINALRELEASE(FinalRelease)
	UIALT_MSG_MAP(UIALT_CALLLESS)
		UIMSG_WM_CREATEBYEDITOR(OnCreateByEditor)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(PropertyCtrl, ITreeView)
  
public:
    void  SetIPropertyCtrl(IPropertyCtrl* p) { 
		m_pIPropertyCtrl = p; 
		SetIMessageProxy(p);
	}
    IPropertyCtrl*  GetIPropertyCtrl(IPropertyCtrl* p) { return m_pIPropertyCtrl; }
        
    IPropertyCtrlGroupItem*  InsertGroupItem(
        LPCTSTR szName, 
        LPCTSTR szDesc, 
        IListItemBase* pParent = UITVI_ROOT, 
        IListItemBase* pInsertAfter = UITVI_LAST, 
        LISTITEM_OPFLAGS nInsertFlags=0);

    IPropertyCtrlGroupItem*  FindGroupItem(IListItemBase* pParentItem, LPCTSTR szName);
    IListItemBase*  FindItemByKey(LPCTSTR szKey);

	IPropertyCtrlEditItem*   InsertEditProperty(
        PropertyCtrlEditItemInfo* pInfo,
        IListItemBase* pParentItem, 
        IListItemBase* pInsertAfter = UITVI_LAST, 
        LISTITEM_OPFLAGS nInsertFlags=0);

    IPropertyCtrlLongItem*   InsertLongProperty(
        PropertyCtrlLongItemInfo* pInfo, 
        IListItemBase* pParentItem, 
        IListItemBase* pInsertAfter = UITVI_LAST, 
        LISTITEM_OPFLAGS nInsertFlags=0);

	IPropertyCtrlBoolItem*   InsertBoolProperty(
		PropertyCtrlBoolItemInfo* pInfo,
		IListItemBase* pParentItem, 
		IListItemBase* pInsertAfter = UITVI_LAST, 
		LISTITEM_OPFLAGS nInsertFlags=0);

	IPropertyCtrlComboBoxItem*   InsertComboBoxProperty(
		PropertyCtrlComboBoxItemInfo* pInfo,
		IListItemBase* pParentItem, 
		IListItemBase* pInsertAfter = UITVI_LAST, 
		LISTITEM_OPFLAGS nInsertFlags=0);

	IPropertyCtrlButtonItem*   InsertButtonProperty(
		LPCTSTR szText, 
		LPCTSTR szValue, 
		LPCTSTR szDesc, 
		LPCTSTR szKey,
		IListItemBase* pParentItem, 
		IListItemBase* pInsertAfter = UITVI_LAST, 
		LISTITEM_OPFLAGS nInsertFlags=0);

	IPropertyCtrlAdditionItem*  AddAdditionItem(LISTITEM_OPFLAGS nFlags);

	IListItemBase*  FindItem(LPCTSTR szKey);
	IPropertyCtrlEditItem*  FindEditItem(LPCTSTR szKey);
    IPropertyCtrlLongItem*  FindLongItem(LPCTSTR szKey);
    IPropertyCtrlComboBoxItem*  FindComboBoxItem(LPCTSTR szKey);
	IPropertyCtrlBoolItem*  FindBoolItem(LPCTSTR szKey);
    // InsertColorProperty
    // Insertxxxx

protected:
    HRESULT  FinalConstruct(IUIApplication* p);
    void  FinalRelease();
	void  OnInitialize();
    void  OnInitialize2();
	void  OnCreateByEditor(CREATEBYEDITORDATA* pData);
    void  OnSize(UINT nType, int cx, int cy);

    LRESULT  OnGetSplitterLinePosPtr(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnGetEditCtrl(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnGetComboBoxCtrl(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnGetButtonCtrl(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnPreShowTooltip(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    IEdit*      m_pEdit;
    IButton*    m_pButton;
    IComboBox*  m_pComboBox;

private:
    IPropertyCtrl*  m_pIPropertyCtrl;
	IPropertyCtrlAdditionItem*  m_pAdditionItem;

    UINT  m_nSplitterLinePercent;   // 分隔线位置，用千分比来表示
    UINT  m_nSplitterLinePos;       // 分隔线位置，px，由percent计算而来
};

}

