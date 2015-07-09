#pragma once
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"
#include "UISDK\Control\Inc\Interface\icombobox.h"
#include "UISDK\Control\Inc\Interface\iedit.h"
#include "UISDK\Control\Inc\Interface\ilistbox.h"
#include "propertyctrlitembase.h"
#include "UISDK\Control\Src\Control\ListBox\listboxitem.h"

namespace UI
{
interface IComboBox;


struct OptionItem
{
	String  strText;
	long lValue;
};

class PropertyCtrlComboBoxItem : public PropertyCtrlItemBase
{
public:
    PropertyCtrlComboBoxItem();
    ~PropertyCtrlComboBoxItem();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMESSAGE_HANDLER_EX(UI_WM_STATECHANGED2, OnStateChanged)
        UIMSG_WM_SIZE(OnSize)
		UIMSG_WM_NOTIFY(UI_CBN_SELCHANGED, OnComboBoxSelChanged)
        UIMSG_WM_NOTIFY(UI_EN_KEYDOWN, OnEditKeyDown)
		UIMESSAGE_HANDLER_EX(UI_PROPERTYCTRLITEM_MSG_SETVALUESTRING, OnSetValue)

        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(PropertyCtrlItemBase)
    UI_DECLARE_QUERY_INTERFACE(PropertyCtrlComboBoxItem);

public:
    void  SetIPropertyCtrlComboBoxItem(IPropertyCtrlComboBoxItem* p)  
    { m_pIPropertyCtrlComboBoxItem = p; }

    void  SetValueString(LPCTSTR szText);
    void  SetValueLong(long lValue);
	void  SetValueType(ComboBoxItemValueType e) { m_eItemValueTpye = e; }
	void  SetReadOnly(bool b) { 
        m_bReadOnly = b; 
    }
    void  SetMultiSel(bool b){
        m_bMultiSel = b;
    }

	void  AddOption(LPCTSTR szText, long lValue);
    OptionItem*  FindOption(long lValue);

    void  FireValueChanged();
    void  OnMultiSelValueChanged();

private:
    void  OnInitialize();
    void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnSize(UINT nType, int cx, int cy);
    LRESULT  OnStateChanged(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnSetValue(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnComboBoxSelChanged(WPARAM w, LPARAM l);
	LRESULT  OnEditKeyDown(WPARAM w, LPARAM l);
    
    virtual  LPCTSTR  GetValueText() override { return m_strValue.c_str(); }

private:
    void  BeginEdit();
    void  CancelEdit();
	void  AcceptEdit();
    void  UpdateValue2Combo();

protected:
    IPropertyCtrlComboBoxItem*  m_pIPropertyCtrlComboBoxItem;

    ComboBoxItemValueType  m_eItemValueTpye;
	bool  m_bReadOnly;
    bool  m_bMultiSel;

    String  m_strDefaultValue;
    long  m_lDefualtValue;

    String  m_strValue;  
    long  m_lValue;
    
    typedef list<OptionItem> _MyList;
    typedef _MyList::iterator _ListIter;
	_MyList  m_mapItems;
};

class MultiSelComboItem;
interface  IMultiSelComboItem : public IListBoxItem
{
    UI_DECLARE_Ixxx_INTERFACE(IMultiSelComboItem, MultiSelComboItem)
};

class MultiSelComboItem : public MessageProxy
{
public:
    MultiSelComboItem();
    ~MultiSelComboItem();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMESSAGE_HANDLER_EX(WM_LBUTTONUP, OnLButtonUp)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(MultiSelComboItem, IListItemBase)

    void  SetIMultiSelComboItem(IMultiSelComboItem* p){
        m_pIMultiSelComboItem = p;
        SetIMessageProxy(p);
    }

    void  SetComboBoxItem(PropertyCtrlComboBoxItem* p)
    {
        m_pBindComboItem = p;
    }

private:
    void  OnPaint(IRenderTarget* pRenderTarget);
    LRESULT  OnLButtonUp(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    IMultiSelComboItem*  m_pIMultiSelComboItem;
    PropertyCtrlComboBoxItem*  m_pBindComboItem;
};

}