#pragma once
#include "UISDK\Control\Inc\Interface\ipropertyctrl.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"
#include "UISDK\Control\Inc\Interface\iedit.h"

namespace UI
{
interface IEdit;

class PropertyCtrlButtonItemShareData : public MessageProxy
{
public:
    PropertyCtrlButtonItemShareData();
    ~PropertyCtrlButtonItemShareData();

    UI_BEGIN_MSG_MAP
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(PropertyCtrlButtonItemShareData, IListItemTypeShareData)

    void  SetIPropertyCtrlButtonItemShareData(IPropertyCtrlButtonItemShareData* p)
    { m_pIPropertyCtrlButtonItemShareData = p; SetIMessageProxy(static_cast<IMessage*>(m_pIPropertyCtrlButtonItemShareData)); }

public:
    IPropertyCtrlButtonItemShareData*  m_pIPropertyCtrlButtonItemShareData;
    UINT*          pSlitterLinePos;
    IRenderFont*   pRenderFont;
    IButton*       pButtonCtrl;
	IEdit*         pEditCtrl;
};

class PropertyCtrlButtonItem : public MessageProxy
{
public:
    PropertyCtrlButtonItem();
    ~PropertyCtrlButtonItem();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMESSAGE_HANDLER_EX(UI_WM_STATECHANGED2, OnStateChanged)
        UIMSG_WM_SIZE(OnSize)
//         UIMSG_WM_NOTIFY(UI_EN_ESC, OnEditEsc)
//         UIMSG_WM_NOTIFY(UI_EN_RETURN, OnEditReturn)
		UIMESSAGE_HANDLER_EX(UI_PROPERTYCTRLITEM_MSG_SETVALUESTRING, OnSetValue)
		UIMESSAGE_HANDLER_EX(UI_PROPERTYCTRLITEM_MSG_SETDEFAULTVALUESTRING, OnSetDefaultValue)
        UIMESSAGE_HANDLER_EX(UI_PROPERTYCTRLITEM_MSG_GETKEYSTRING, OnGetKey)

        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(PropertyCtrlButtonItem, IListItemBase)

    UI_DECLARE_QUERY_INTERFACE(PropertyCtrlButtonItem);

public:
    void  SetIPropertyCtrlButtonItem(IPropertyCtrlButtonItem* p)  
    { m_pIPropertyCtrlButtonItem = p; SetIMessageProxy(static_cast<IMessage*>(p)); }

    void  SetValueText(const TCHAR* szText);
	void  SetDefaultValueText(const TCHAR* szText);
	void  SetKeyText(const TCHAR* szText);

protected:
    void  OnInitialize();
    void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnSize(UINT nType, int cx, int cy);
    LRESULT  OnStateChanged(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnSetValue(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnSetDefaultValue(UINT uMsg, WPARAM wParam, LPARAM lParam);
//     LRESULT  OnEditEsc(WPARAM w, LPARAM l);
//     LRESULT  OnEditReturn(WPARAM w, LPARAM l);
    LRESULT  OnGetKey(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    void  BeginEdit();
    void  CancelEdit();
    void  GetValueColumnRect(CRect* prc);

private:
    IPropertyCtrlButtonItem*  m_pIPropertyCtrlButtonItem;
    PropertyCtrlButtonItemShareData*  m_pShareData;

    String  m_strValue;  // 该项在xml中的属性值
	String  m_strDefautValue;  // 该项的默认值
	String  m_strKey;    // 该项在xml中的属性字段名称, m_strText表示该项在控件上面显示的值，不一定是xml中的值
};

}