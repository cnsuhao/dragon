#pragma once
#include "UISDK\Control\Inc\Interface\iheaderctrl.h"
namespace UI
{
// 普通自绘item，不涉及子控件
class HeaderListItem : public MessageProxy
{
public:
    HeaderListItem();
    ~HeaderListItem();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMESSAGE_HANDLER_EX(UI_HDM_GETITEM_COLUMNINFO, OnGetColumnInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(HeaderListItem, IListItemBase)

    UI_DECLARE_QUERY_INTERFACE(HeaderListItem);

public:
    void  SetIHeaderListItem(IHeaderListItem* p);
    IHeaderListItem*  GetIHeaderListItem();
    void  SetTextAlign(int n);
    int   GetTextAlign();
    
    void  SetColumnInfo(ColumnInfo* pInfo);
    void  GetColumnInfo(ColumnInfo* pInfo);

protected:
    void  OnInitialize();
    void  OnPaint(UI::IRenderTarget* pRenderTarget);
    LRESULT  OnGetColumnInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
    IHeaderListItem*  m_pIHeaderListItem;
    HeaderListCtrl*  m_pHeaderCtrl;

    short  m_nMinWidth;
    short  m_nMaxWidth;
    short  m_nIndentLeft;
    short  m_nIndentRight;

    int    m_nFormat;
    COLUMN_WIDTH_TYPE  m_eWidthType;
};
}