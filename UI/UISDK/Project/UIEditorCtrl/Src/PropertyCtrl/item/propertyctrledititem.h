#pragma once
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"
#include "UISDK\Control\Inc\Interface\iedit.h"
#include "propertyctrlitembase.h"

namespace UI
{
interface IEdit;

class PropertyCtrlEditItem : public PropertyCtrlItemBase
{
public:
    PropertyCtrlEditItem();
    ~PropertyCtrlEditItem();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_SIZE(OnSize)
		UIMESSAGE_HANDLER_EX(UI_PROPERTYCTRLITEM_MSG_SETVALUESTRING, OnSetValue)
		UIMESSAGE_HANDLER_EX(UI_PROPERTYCTRLITEM_MSG_SETDEFAULTVALUESTRING, OnSetDefaultValue)
		UIMSG_WM_NOTIFY(UI_EN_ACCEPTEDIT, OnAcceptEdit)

        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(PropertyCtrlItemBase)

    UI_DECLARE_QUERY_INTERFACE(PropertyCtrlEditItem);

public:
    void  SetIPropertyCtrlEditItem(IPropertyCtrlEditItem* p)  
    { 
        m_pIPropertyCtrlEditItem = p; 
        SetIMessageProxy(static_cast<IMessage*>(p)); 
    }

    void  SetValueText(const TCHAR* szText);
	void  SetDefaultValueText(const TCHAR* szText);

protected:
    void  OnInitialize();
    void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnSize(UINT nType, int cx, int cy);
	LRESULT  OnSetValue(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnSetDefaultValue(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnAcceptEdit(WPARAM wParam, LPARAM lParam);

protected:
    void  BeginEdit();
    void  EndEdit();
    void  AcceptEdit();

	virtual void  OnValueColumnClick(POINT ptWnd, POINT ptItem) override;
    virtual LPCTSTR  GetValueText() override { return m_strValue.c_str(); }

protected:
	String  m_strValue;  // 该项在xml中的属性值
	String  m_strDefautValue;  // 该项的默认值

private:
    IPropertyCtrlEditItem*  m_pIPropertyCtrlEditItem;
};

}