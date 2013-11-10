#pragma once
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"

class BigfaceLayout : public UI::IListCtrlLayout
{
public:
    BigfaceLayout() { m_pIListCtrlBase = NULL; }

    virtual void  SetIListCtrlBase(UI::IListCtrlBase* p){ m_pIListCtrlBase = p; };
    virtual void  Arrange(UI::IListItemBase* pStartToArrange, SIZE* pSizeContent);
    virtual void  Measure(SIZE* pSize);
    virtual void  Release();

    int  GetItemHeight(UI::IListItemBase*  pItem);

private:
    UI::IListCtrlBase*  m_pIListCtrlBase;
};

