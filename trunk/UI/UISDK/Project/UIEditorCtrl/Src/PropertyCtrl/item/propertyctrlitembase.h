#pragma once
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"
#include "UISDK\Project\UIEditorCtrl\Inc\ipropertyctrl.h"

namespace UI
{
interface IButton;
interface IEdit;
interface IComboBox;

class PropertyCtrlItemBaseShareData : public MessageProxy
{
public:
    PropertyCtrlItemBaseShareData();

    UI_BEGIN_MSG_MAP
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(PropertyCtrlItemBaseShareData, IListItemTypeShareData)

    void  SetIPropertyCtrlItemBaseShareData(IPropertyCtrlItemBaseShareData* p)
    {
        m_pIPropertyCtrlItemBaseShareData = p; 
        SetIMessageProxy(static_cast<IMessage*>(m_pIPropertyCtrlItemBaseShareData)); 
    }
    uint  GetSplitterLinePos();
    void  SetSplitterLinePosPtr(uint* p) { m_pSlitterLinePos = p; }

public:
    IPropertyCtrlItemBaseShareData*  m_pIPropertyCtrlItemBaseShareData;
    IRenderFont*  m_pRenderFont;
    IButton*  pButtonCtrl;
    IEdit*  pEditCtrl;
    IComboBox*  pComboBoxCtrl;
private:
    uint*  m_pSlitterLinePos;
};


class PropertyCtrlItemBase : public MessageProxy
{
public:
    PropertyCtrlItemBase();
    ~PropertyCtrlItemBase();

    enum MAGIC_NUMBER
    {
        TEXT_INDENT = 5,
        SPLITLINE_WIDTH = 1,
    };

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
		UIMESSAGE_HANDLER_EX(UI_PROPERTYCTRLITEM_MSG_GETKEYSTRING, OnGetKey)
        UIMESSAGE_HANDLER_EX(UI_PROPERTYCTRLITEM_MSG_GETVALUESTRING, OnGetValue)
		UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(PropertyCtrlItemBase, IListItemBase)

public:
    void  SetIPropertyCtrlItemBase(IPropertyCtrlItemBase* p)  
    {
        m_pIPropertyCtrlItemBase = p;
        SetIMessageProxy(static_cast<IMessage*>(p)); 
    }

	void  SetKey(LPCTSTR szText);
    LPCTSTR  GetKey();
    void  GetValueColumnRectInCtrl(LPRECT prc);
    void  GetKeyColoumnRectInCtrl(LPRECT prc);
    void  GetValueColumnRectInItem(LPRECT prc);
	void  GetKeyColoumnRectInItem(LPRECT prc);

	bool  IsEditActive();
    bool  IsComboBoxActive();

protected:
	virtual void  OnValueColumnClick(POINT ptWnd, POINT ptItem){};
    virtual LPCTSTR  GetValueText() { return NULL; }

private:
    void  OnInitialize();
    void  OnPaint(IRenderTarget* pRenderTarget);
	void  OnLButtonDown(UINT nFlags, POINT point);
	LRESULT  OnGetKey(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnGetValue(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    PropertyCtrlItemBaseShareData*  m_pBaseShareData;
    String  m_strKey;    // 该项在xml中的属性字段名称, m_strText表示该项在控件上面显示的值，不一定是xml中的值

private:
    IPropertyCtrlItemBase*  m_pIPropertyCtrlItemBase;
	
};

}