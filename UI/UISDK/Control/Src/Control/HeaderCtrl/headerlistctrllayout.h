#pragma once
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"
namespace UI
{
class HeaderListItemLayout : public IListCtrlLayout
{
public:
    HeaderListItemLayout(bool bPercentMode) 
    {
        m_bPercentMode = bPercentMode;
        m_pIListCtrlBase = NULL; 
    }

    static HeaderListItemLayout*  CreateInstance(bool bPercentMode) 
    {
        return new HeaderListItemLayout(bPercentMode); 
    }
    virtual void  SetIListCtrlBase(IListCtrlBase* p)
    {
        m_pIListCtrlBase = p; }
    ;
    virtual void  Arrange(IListItemBase* pStartToArrange, 
        SIZE* pSizeContent);
    virtual void  Measure(SIZE* pSize);
    virtual void  Release();

protected:
    int  GetItemWidth(IListItemBase*  pItem);
    void  ArrangeByPercent(IListItemBase* pStartToArrange, 
        SIZE* pSizeContent);

private:
    UI::IListCtrlBase*  m_pIListCtrlBase;
    bool  m_bPercentMode;  // ���ٷֱ�ģʽ���ָ������û�к������
};
}