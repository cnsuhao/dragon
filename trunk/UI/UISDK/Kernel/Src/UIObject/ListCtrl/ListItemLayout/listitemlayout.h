#ifndef LISTITEMLAYOUT_H_6F45FF31_A2CF_4c4d_B411_899A5CC12D61
#define LISTITEMLAYOUT_H_6F45FF31_A2CF_4c4d_B411_899A5CC12D61
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"

namespace UI
{
interface IListItemBase;
interface IListCtrlBase;
class ListCtrlBase;

enum LISTCTRL_ITEM_LAYOUT_TYPE
{
    // 1. 普通系统listbox. [固定行高]
    //    滚动条：  横向NONE，纵向AUTO
    //    布  局：  横向占满client width，纵向依次排列
    //    AUTOSIZE: 横向0(不支持AUTO), 纵向total height
    LISTCTRL_ITEM_LAYOUT_TYPE_1,

    // 2. 普通系统listbox，支持横向滚动. [固定行高]
    //    滚动条：  横向AUTO，纵向AUTO
    //    布  局：  横向取max width，或者client width，纵向依次排列
    //    AUTOSIZE: 横向max width, 纵向total height
    LISTCTRL_ITEM_LAYOUT_TYPE_2,
};


class  ListCtrlLayoutMgr
{
public:
    ListCtrlLayoutMgr();
    ~ListCtrlLayoutMgr();
    
    void  SetListCtrlBase(ListCtrlBase* pListCtrlBase);

    IListCtrlLayout*  GetLayout();
    void  SetLayout(IListCtrlLayout* p);

    bool  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent);
    void  Measure(SIZE* pSize);

private:
    ListCtrlBase*  m_pListCtrl;
    IListCtrlLayout*  m_pLayout;
};


class ListCtrlItemSimpleLayout : public IListCtrlLayout
{
public:
    ListCtrlItemSimpleLayout();

    virtual void  SetIListCtrlBase(IListCtrlBase* p) { m_pIListCtrlBase = p; }
    virtual void  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent);
    virtual void  Measure(SIZE* pSize);
    virtual void  Release() { delete this; }

protected:
    IListCtrlBase*  m_pIListCtrlBase;
};

class ListCtrlItemVariableHeightLayout : public IListCtrlLayout
{
public:
    ListCtrlItemVariableHeightLayout();

    virtual void  SetIListCtrlBase(IListCtrlBase* p) { m_pIListCtrlBase = p; }
    virtual void  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent);
    virtual void  Measure(SIZE* pSize);
    virtual void  Release() { delete this; }

protected:
    IListCtrlBase*  m_pIListCtrlBase;
};


// TAB布局，横向平均排布
class ListCtrlItemHorzAverageLayout : public IListCtrlLayout
{
    virtual void  SetIListCtrlBase(IListCtrlBase* p) { m_pIListCtrlBase = p; }
    virtual void  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent);
    virtual void  Measure(SIZE* pSize){};
    virtual void  Release() { delete this; }

protected:
    IListCtrlBase*  m_pIListCtrlBase;
};


#define UI_MSGMAPID_LC_LAYOUT 142901215

// 向列表控件获取行间距
#define UI_LCM_LAYOUT_GET_LINESPACE  142901216
// 向列表控件获取item间距
#define UI_LCM_LAYOUT_GET_ITEMSPACE  142901217

// 等高，流式布局，一行铺满则换行 （只有纵向滚动条）
class ListCtrlItemFixHeightFlowLayout : public IListCtrlLayout
{
    virtual void  SetIListCtrlBase(IListCtrlBase* p) { m_pIListCtrlBase = p; }
    virtual void  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent);
    virtual void  Measure(SIZE* pSize);
    virtual void  Release() { delete this; }

protected:
    IListCtrlBase*  m_pIListCtrlBase;
};


IListCtrlLayout*  CreateListCtrlLayout(int nType, IListCtrlBase* p);
}

#endif // LISTITEMLAYOUT_H_6F45FF31_A2CF_4c4d_B411_899A5CC12D61