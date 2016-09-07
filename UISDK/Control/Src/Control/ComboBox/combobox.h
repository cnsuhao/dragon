#pragma once
#include "UISDK\Control\Inc\Interface\icombobox.h"
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"
#include "UISDK\Control\Inc\Interface\iedit.h"

namespace UI
{

interface IEdit;
interface IButton;
interface IListBox;
class PopupListBoxWindow;

//
// 需要在皮肤中多配置一个下拉列表
/*
 <class id="popup.layout.fill" layout.align="forcedesiredsize" 
      layout.left="0" layout.top="0" layout.right="0" layout.bottom="0"/>
 <CustomWindow id="DefaultComboBoxDropWnd">
 	<ListBox styleclass="popup.layout.fill"/>
 </CustomWindow>
*/
//

// 外部调用comboboxbase->SetText后，给派生类一个机会做进一步的处理。例如更新列表当前选项
// wParam: LPCTSTR szText
#define COMBOBOX_MSG_SETTEXT  153082256

class ComboBoxBase : public MessageProxy
{
public:
	ComboBoxBase();
	~ComboBoxBase();

	UI_BEGIN_MSG_MAP
		UIMSG_WM_STATECHANGED2(OnStateChanged)
		UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
		UIMSG_WM_REDRAWOBJECT(OnRedrawObject)
		UIMSG_WM_SIZE(OnSize)
		UIMSG_WM_STYLECHANGED(OnStyleChanged)
		UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
        UIMSG_WM_LBUTTONDBLCLK(OnLButtonDown)
		UIMSG_WM_INITPOPUPCONTROLWINDOW(OnInitPopupControlWindow)
		UIMSG_WM_UNINITPOPUPCONTROLWINDOW(OnUnInitPopupControlWindow)
        UIMSG_WM_SERIALIZE(OnSerialize)
        UIMSG_WM_INITIALIZE(OnIntialize)
        UIMSG_WM_FINALRELEASE(FinalRelease)
		UIMSG_WM_FINALCONSTRUCT(FinalConstruct)

	UIALT_MSG_MAP(COMBOBOX_BUTTON_MSG_HOOK_MSG_ID)  // Button hook
		UIMSG_WM_LBUTTONDOWN(OnBtnLButtonDown)
		UIMSG_WM_STATECHANGED2(OnChildObjStateChanged)

	UIALT_MSG_MAP(COMBOBOX_EDIT_MSG_HOOK_MSG_ID)
		UIMSG_WM_STATECHANGED2(OnChildObjStateChanged)
		UIMSG_WM_NOTIFY(UI_EN_KEYDOWN, OnEditKeyDown)

    UIALT_MSG_MAP(UIALT_CALLLESS)
        UIMSG_WM_CREATEBYEDITOR(OnCreateByEditor)

	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ComboBoxBase, IControl)

    void  SetIComboBoxBase(IComboBoxBase* p) 
    { 
        m_pIComboBoxBase = p; 
        SetIMessageProxy(static_cast<IMessage*>(p)); 
    }
    IComboBoxBase*  GetIComboBoxBase() 
    { 
        return m_pIComboBoxBase; 
    }

public:
    void  CloseUp();
	void  DropDown();
	bool  IsDropDown();

    IEdit*  GetEdit() { return m_pEdit; }
    LPCTSTR  GetText();
	void  SetText(LPCTSTR szText, bool bUpdate);
    void  SetReadOnly(bool b);
    bool  IsReadOnly();

	IObject*  GetContentObject(LPCTSTR szText);

protected:
    void  SetDropWndId(LPCTSTR);
    LPCTSTR  GetDropWndId();

private:
	HRESULT  FinalConstruct(IUIApplication* p);
    void  OnIntialize();
    void  FinalRelease();
    void  OnCreateByEditor(CREATEBYEDITORDATA* pData);
	void  OnSerialize(SERIALIZEDATA* pData);
	void  GetAutoSize(SIZE* pSize);

	void  OnStateChanged(UINT nMask);
	void  OnChildObjStateChanged(UINT nMask);
	void  OnRedrawObject();
	LRESULT  OnEditKeyDown(WPARAM wParam, LPARAM);

	void  OnEraseBkgnd(IRenderTarget* pRenderTarget);
	void  OnSize(UINT nType, int cx, int cy);
	void  OnLButtonDown(UINT nFlags, POINT point);
	void  OnBtnLButtonDown(UINT nFlags, POINT point);
	void  OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	
	void  OnInitPopupControlWindow();
	void  OnUnInitPopupControlWindow();

    void  OnDrawStaticCurSel(
            IRenderTarget* pRenderTarget, 
            ITextRenderBase* pTextRender);

    bool  defaultDropDown();
	void  destroyDropWnd();
	PopupListBoxWindow*  GetDropWnd();

protected:
    IComboBoxBase*  m_pIComboBoxBase;
	IEdit*  m_pEdit;
	IButton*  m_pButton;
	String  m_strDropWndId; // 下拉列表窗口皮肤id

	bool  m_bEditDropList; // 是否是可编辑的combobox
    
private:
	PopupListBoxWindow*  m_pPopupWnd;
};

class ComboBox : public ComboBoxBase
{
public:
    ComboBox();
    UI_DECLARE_OBJECT(ComboBox, OBJ_CONTROL|CONTROL_COMBOBOX);

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_NOTIFY(UI_CBN_DROPDOWN, OnDropDown)
		UIMESSAGE_HANDLER_EX(COMBOBOX_MSG_SETTEXT, OnSetText)
        UIMSG_WM_INITPOPUPCONTROLWINDOW(OnInitPopupControlWindow)
		UIMSG_WM_UNINITPOPUPCONTROLWINDOW(OnUnInitPopupControlWindow)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_FINALRELEASE(FinalRelease)
    
    UIALT_MSG_MAP(COMBOBOX_LISTBOX_MSG_HOOK_MSG_ID)  // ListBox Notify
        UIMSG_LCN_SELCHANGED(OnLCNSelChanged)
        UIMSG_WM_NOTIFY(UI_NM_CLICK, OnListCtrlClick)
		UIMSG_WM_NOTIFY(UI_NM_KEYDOWN, OnListCtrlKeyDown)

	UIALT_MSG_MAP(COMBOBOX_EDIT_MSG_HOOK_MSG_ID)
		UIMSG_WM_NOTIFY(UI_EN_CHANGE, OnEditChanged)

    UI_END_MSG_MAP_CHAIN_PARENT(ComboBoxBase)

public:
    void  SetIComboBox(IComboBox* p) { m_pIComboBox = p; }
    IComboBox*  GetIComboBox() { return m_pIComboBox; }

    bool  AddString(LPCTSTR, int nAddItemFlags=0);
    IListBoxItem*  AddStringEx(LPCTSTR, int nAddItemFlags=0);
    bool  SetCurSel(int nIndex);
    IListBox*  GetListBox();
	void  ResetContent();

private:
    void  OnPaint(IRenderTarget*);
    void  FinalRelease();
    HRESULT  OnDropDown(WPARAM, LPARAM);
	LRESULT  OnSetText(UINT, WPARAM, LPARAM);
    void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
    void  OnLCNSelChanged(IMessage* pMsgFrom, IListItemBase* pOldSelItem, IListItemBase* pSelItem);
    LRESULT  OnListCtrlClick(WPARAM, LPARAM);
	LRESULT  OnListCtrlKeyDown(WPARAM, LPARAM);
	LRESULT  OnEditChanged(WPARAM wParam, LPARAM);
    void  OnInitPopupControlWindow();
	void  OnUnInitPopupControlWindow();
	

private:
    IComboBox*  m_pIComboBox;

	// 该变量是延迟创建的。要访问该变量，通过GetListBox()获得
    IListBox*   m_pListCtrl; 
};


}