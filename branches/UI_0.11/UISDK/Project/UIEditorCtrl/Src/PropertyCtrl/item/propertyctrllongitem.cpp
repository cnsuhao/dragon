#include "stdafx.h"
#include "propertyctrlLongItem.h"
#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Kernel\Inc\Interface\ilayout.h"

using namespace UI;


//////////////////////////////////////////////////////////////////////////

PropertyCtrlLongItem::PropertyCtrlLongItem()
{
    m_pIPropertyCtrlLongItem = NULL;
    m_lValue = 0;
	m_lDefaultValue = 0;
}
PropertyCtrlLongItem::~PropertyCtrlLongItem()
{
}

void  PropertyCtrlLongItem::OnInitialize()
{
    DO_PARENT_PROCESS(IPropertyCtrlLongItem, IPropertyCtrlItemBase);
	m_pIPropertyCtrlLongItem->SetItemType(IPropertyCtrlLongItem::FLAG);
}

void  PropertyCtrlLongItem::SetValue(long lValue)
{
    m_lValue = lValue;

	TCHAR szText[32] = {0};
	_stprintf(szText, TEXT("%d"), lValue);

	SetValueText(szText);
}
