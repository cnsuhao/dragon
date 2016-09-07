#pragma once
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"
#include "UISDK\Control\Inc\Interface\iedit.h"
#include "propertyctrledititem.h"

namespace UI
{

class PropertyCtrlLongItem : public PropertyCtrlEditItem
{
public:
    PropertyCtrlLongItem();
    ~PropertyCtrlLongItem();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(PropertyCtrlEditItem)

    UI_DECLARE_QUERY_INTERFACE(PropertyCtrlLongItem)

public:
    void  SetIPropertyCtrlLongItem(IPropertyCtrlLongItem* p)  
    {
        m_pIPropertyCtrlLongItem = p; 
    }

    void  SetValue(long lValue);

private:
	void  OnInitialize();

private:
    IPropertyCtrlLongItem*  m_pIPropertyCtrlLongItem;
    long  m_lValue;
	long  m_lDefaultValue;
};

}