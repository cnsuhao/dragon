#ifndef IHEADERCTRL_H_A101851E_A3A9_4db0_8A64_D1C9A87A1ABA
#define IHEADERCTRL_H_A101851E_A3A9_4db0_8A64_D1C9A87A1ABA

#include "UISDK\Kernel\Inc\Interface\icontrol.h"
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"

namespace UI
{

    // Message/Notify

    // 通知开始/正在/结束拖拽分隔条
    // Notifies a header control's parent window that the user is dragging a divider in the header control
    //       message: UI_WM_NOTIFY
    //       code:    UI_HDN_BEGIN_TRACK
    //       wparam:  nColumnIndex
    //       lparam:
    //       Return: Returns FALSE（默认允许）to allow tracking of the divider, or TRUE to prevent tracking. 
#define UI_HDN_BEGIN_TRACK 136131135

        //       message: UI_WM_NOTIFY
        //       code:    UI_HDN_TRACKING
        //       wparam:  nColumnIndex
        //       lparam:  int* [in,out] pnColumnNewWidth
        //       return:  BOOL, TRUE表示已处理；FALSE表示未处理，内部将自己来刷新
#define UI_HDN_TRACKING 136131136

        // 同UI_HDCN_BEGIN_TRACK，无返回值
#define UI_HDN_END_TRACK 136131137

        // 双击分隔条
        // Notifies a header control's parent window that the user double-clicked the divider area of the control. 
        //		message: UI_WM_NOTIFY
        //      code:    UI_HDN_DIVIDER_DBLCLICK
        //      wparam:  nColumnIndex
        //      lparam:
        //      return:
        //
#define UI_HDN_DIVIDER_DBLCLICK 136131138

        // header ctrl宽度发生改变的通知消息
        //      message: UI_WM_NOTIFY
        //      code:    UI_HDN_TOTAL_WIDTH_CHANGED
        //      wparam:  int nTotalWidth
        //      lparam:
        //      return:
#define UI_HDN_TOTAL_WIDTH_CHANGED 136131139
        // 
        // 	UI_HDCN_ITEM_CLICK,
        // 
        // 	UI_HDCN_ITEM_DBLCLICK,
        // 
        // 	UI_HDCN_ITEM_RCLICK,

    // wParam: ColumnInfo*
#define  UI_HDM_GETITEM_COLUMNINFO 136131150

// 新增列
// wParam: IListItemBase*
#define  UI_HDN_ADDCOLUMN  157081214
// 删除列
// wParam: IListItemBase*
#define  UI_HDN_REMOVECOLUMN  157081215
// 清空列
#define  UI_HDN_CLEARCOLUMN  157081216

// void  OnAddColumn(IListItemBase* pAddColumn);
#define UIMSG_HDN_ADDCOLUMN(func)                     \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_HDN_ADDCOLUMN)                     \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((UI::IListItemBase*)wParam);             \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void  OnRemoveColumn(IListItemBase* pRemoveColumn);
#define UIMSG_HDN_REMOVECOLUMN(func)                  \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_HDN_REMOVECOLUMN)                  \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((UI::IListItemBase*)wParam);             \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void  OnClearColumn();
#define UIMSG_HDN_CLEARCOLUMN(func)                   \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_HDN_CLEARCOLUMN)                   \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func();                                       \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// BOOL OnHeaderCtrlTracking(int nColumnIndex, int* pNewWidth);
#define UIMSG_HDN_TRACKING(func)                      \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_HDN_TRACKING )                     \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        lResult = func((int)wParam, (int*)lParam);    \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void OnHeaderCtrlTotalWidthChanged(int nTotalWidth);
#define UIMSG_HDN_TOTAL_WIDTH_CHANGED(func)           \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_HDN_TOTAL_WIDTH_CHANGED)           \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((int)wParam);                            \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// LRESULT OnHeaderCtrlBeginTrack(int nColumnIndex);
#define UIMSG_HDN_BEGIN_TRACK(func)                   \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_HDN_BEGIN_TRACK)                   \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        lResult = func((int)wParam);                  \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }


    // 其它定义
#define HEADER_DIVIDER_EXTENT_SIZE 10  // 拖拽宽度


enum  COLUMN_WIDTH_TYPE
{
    COLUMN_WIDTH_NORMAL,  // 传统模式，由鼠标拖拽大小决定
    COLUMN_WIDTH_FIXED,   // 固定，大小不变
    COLUMN_WIDTH_PERCENT, // 按比例占用剩余空间
};


#define CIM_ORDER  0x0001
#define CIM_ID     0x0002
#define CIM_TEXT   0x0004
#define CIM_WIDTH  0x0008
#define CIM_WIDTH_TYPE  0x0010
#define CIM_MIN_WIDTH   0x0020
#define CIM_MAX_WIDTH   0x0040
#define CIM_FORMAT 0x0080
#define CIM_INDENT_LEFT  0x0100
#define CIM_INDENT_RIGHT 0x0200
#define CIM_ALL  0xFFFF

struct ColumnInfo
{
    int      nMask;

    int      nOrder;
    int      nColumnId;
    LPCTSTR  szText;
    int      nWidth;
    COLUMN_WIDTH_TYPE  eWidthType;
    short    nMinWidth;
    short    nMaxWidth;
    int      nFormat;
    int      nIndentLeft;
    int      nIndentRight;
};

//////////////////////////////////////////////////////////////////////////
class HeaderListItem;
interface UICTRLAPI IHeaderListItem : public IListItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(IHeaderListItem, HeaderListItem);
    
    void  SetTextAlign(int n);
    int   GetTextAlign();

    void  GetColumnInfo(ColumnInfo* pInfo);
    void  SetColumnInfo(ColumnInfo* pInfo);
};

class HeaderListCtrl;
interface UICTRLAPI IHeaderListCtrl : public IListCtrlBase
{
     UI_DECLARE_Ixxx_INTERFACE(IHeaderListCtrl, HeaderListCtrl);

     void  SetBindControl(IMessage*);
     IMessage*  GetBindControl();

     IHeaderListItem*  AddColumn(LPCTSTR szText, int nWidth, int nFormat=DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS);
     IHeaderListItem*  InsertColumn(int nPos, LPCTSTR szText, int nWidth, int nFormat=DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS);

     int   GetColumnCount();
     bool  CanDragColumn();
     void  SetHScrollOffset(int nhScrollOffset); 
     int   GetHScrollOffset();
     int   GetColumnTotalWidth();
};
}

#endif // IHEADERCTRL_H_A101851E_A3A9_4db0_8A64_D1C9A87A1ABA