#pragma once
#include "UISDK\Project\UIEditorCtrl\Inc\ipropertyctrl.h"

namespace UI
{

class PropertyCtrlGroupItem : public MessageProxy
{
public:
    PropertyCtrlGroupItem();
    ~PropertyCtrlGroupItem();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
		UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(PropertyCtrlGroupItem, INormalTreeItem)

    UI_DECLARE_QUERY_INTERFACE(PropertyCtrlGroupItem);

public:
    void  SetIPropertyCtrlGroupItem(IPropertyCtrlGroupItem* p)  
    { 
		m_pIPropertyCtrlGroupItem = p; 
		SetIMessageProxy(p);
	}

protected:
    void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnLButtonDown(UINT nFlags, POINT point);
	void  OnInitialize();
private:
	IPropertyCtrlGroupItem*  m_pIPropertyCtrlGroupItem;
	INormalTreeItemShareData*  m_pShareData;
};

}