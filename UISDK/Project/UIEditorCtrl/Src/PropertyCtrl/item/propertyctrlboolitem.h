#pragma once
#include "propertyctrlcomboboxitem.h"

namespace UI
{

class PropertyCtrlBoolItem : public PropertyCtrlComboBoxItem
{
public:
    PropertyCtrlBoolItem();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
		UIMSG_WM_QUERYINTERFACE(QueryInterface)
    UI_END_MSG_MAP_CHAIN_PARENT(PropertyCtrlComboBoxItem)   
	UI_DECLARE_QUERY_INTERFACE(PropertyCtrlBoolItem)

public:
    void  SetIPropertyCtrlBoolItem(IPropertyCtrlBoolItem* p)  
    { m_pIPropertyCtrlBoolItem = p; }

    void  SetBool(bool b);
    void  SetDefault(bool b);
    bool  GetBool();

private:
    void  OnLButtonDblClk(UINT nFlags, POINT point);

private:
    IPropertyCtrlBoolItem*  m_pIPropertyCtrlBoolItem;

};

}