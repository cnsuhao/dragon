#pragma once
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"
#include "UISDK\Control\Inc\Interface\iedit.h"
#include "propertyctrlitembase.h"

namespace UI
{
class PropertyCtrlButtonItem : public PropertyCtrlItemBase
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
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(PropertyCtrlItemBase)

    UI_DECLARE_QUERY_INTERFACE(PropertyCtrlButtonItem);

public:
    void  SetIPropertyCtrlButtonItem(IPropertyCtrlButtonItem* p)  
    { m_pIPropertyCtrlButtonItem = p; SetIMessageProxy(static_cast<IMessage*>(p)); }

    void  SetValueText(const TCHAR* szText);
	void  SetDefaultValueText(const TCHAR* szText);

protected:
    void  OnInitialize();
    void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnSize(UINT nType, int cx, int cy);
    LRESULT  OnStateChanged(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnSetValue(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnSetDefaultValue(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    void  BeginEdit();
    void  CancelEdit();

private:
    IPropertyCtrlButtonItem*  m_pIPropertyCtrlButtonItem;

    String  m_strValue;  // ������xml�е�����ֵ
	String  m_strDefautValue;  // �����Ĭ��ֵ
};

}