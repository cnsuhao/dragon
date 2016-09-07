#pragma once
#include "treeitem.h"

namespace UI
{
// 操作系统样式的树结点，里面的元素都是绘制出来的
class NormalTreeItemShareData : public MessageProxy
{
public:
    NormalTreeItemShareData();
    ~NormalTreeItemShareData();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(NormalTreeItemShareData, IListItemTypeShareData)

    void  SetINormalTreeItemShareData(INormalTreeItemShareData* p)
    { m_pINormalTreeItemShareData = p; SetIMessageProxy(static_cast<IMessage*>(p)); }

public:
    int  GetImageWidth()
    {
        return nImageWidth;
    }
    int  GetExpandIconWidth()
    {
        return nExpandIconWidth;
    }
protected:
    void  ResetAttribute();

public:
    INormalTreeItemShareData*  m_pINormalTreeItemShareData;
    IRenderBase*  pExpandIconRender;

    int  nExpandIconWidth;
    int  nImageWidth;
};

class NormalTreeItem : public TreeItem
{
public:
    NormalTreeItem();
    void  SetINormalTreeItem(INormalTreeItem* p) { m_pINormalTreeItem = p; }

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_INITIALIZE(OnInitialize)
        UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
        UIMSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(NormalTreeItem, ITreeItem)

protected:
    void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnInitialize();
    void  OnLButtonDown(UINT nFlags, POINT point);
    void  OnLButtonDblClk(UINT nFlags, POINT point);
    void  GetDesiredSize(SIZE* pSize);

protected:
    void  GetItemExpandIconRect(CRect* prc);
    void  DrawLine(IRenderTarget* pRenderTarget);
    void  DrawFullVLine(HDC hDC, const CRect* prc);
    void  DrawFullHLine(HDC hDC, const CRect* prc);
    void  DrawHalfVTopLine(HDC hDC, const CRect* prc);
    void  DrawHalfVBottomLine(HDC hDC, const CRect* prc);
    void  DrawHalfHLine(HDC hDC, const CRect* prc);

private:
    INormalTreeItem*  m_pINormalTreeItem;
protected:
    NormalTreeItemShareData*  m_pShareData;
};
}