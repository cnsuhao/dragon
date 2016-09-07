#include "stdafx.h"
#include "float_item_mgr.h"
#include "..\ListItemBase\listitembase.h"
#include "..\listctrlbase.h"

using namespace UI;

FloatItemMgr::FloatItemMgr()
{
    m_pListCtrl = NULL;    
}

FloatItemMgr::~FloatItemMgr()
{
    
}

void  FloatItemMgr::SetListCtrlBase(ListCtrlBase* p)
{
    m_pListCtrl = p;
}

void  FloatItemMgr::OnRemoveItem(ListItemBase* pItem)
{
    _mapiter iter = m_mapFloatItems.begin();
    for (; iter != m_mapFloatItems.end();)
    {
        if (pItem == iter->first || pItem->IsMyChildItem(iter->first, true))
        {
            iter = m_mapFloatItems.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void  FloatItemMgr::OnRemoveAllItem()
{
    m_mapFloatItems.clear();
}

void  FloatItemMgr::SetItemFloatRect(ListItemBase* pItem, RECT* prcFloat)
{
    if (!pItem)
        return;

    FloatItemData& data = add_float_item(pItem);
    CopyRect(&data.rcFloat, prcFloat);
}

void  FloatItemMgr::AddFloatItem(ListItemBase* p)
{
    if (!p)
        return;

    add_float_item(p);
}
void  FloatItemMgr::RemoveFloatItem(ListItemBase* pItem)
{
    if (!pItem)
        return;

    _mapiter iter = m_mapFloatItems.find(pItem);
    if (iter == m_mapFloatItems.end())
        return;

    ListItemStyle s = {0};
    s.bFloat = 1;
    pItem->ModifyStyle(NULL, &s);
    
    m_mapFloatItems.erase(iter);
}

FloatItemData&  FloatItemMgr::add_float_item(ListItemBase* pItem)
{
    UIASSERT(pItem);
    _mapiter iter = m_mapFloatItems.find(pItem);
    if (iter != m_mapFloatItems.end())
        return iter->second;

    // 增加post paint，绘制浮动元素
    if (m_mapFloatItems.empty())
    {
        ObjStyle s2 = {0};
        s2.post_paint = 1;
        m_pListCtrl->ModifyObjectStyle(&s2, NULL);
    }

    FloatItemData  data = {0};
    data.pItem = pItem;
    m_mapFloatItems[pItem] = data;

    {
        ListItemStyle s = {0};
        s.bFloat = 1;
        pItem->ModifyStyle(&s, NULL);
    }

    return m_mapFloatItems[pItem];
}

bool  FloatItemMgr::GetItemFloatRect(ListItemBase* pItem, RECT* prcFloat)
{
    if (!pItem)
        return false;

    _mapiter iter = m_mapFloatItems.find(pItem);
    if (iter == m_mapFloatItems.end())
        return false;

    CopyRect(prcFloat, &iter->second.rcFloat);
    return true;
}

void  FloatItemMgr::DoPaint(IRenderTarget* pRenderTarget)
{
    if (m_mapFloatItems.empty())
        return;

    _mapiter iter = m_mapFloatItems.begin();
    for (; iter != m_mapFloatItems.end(); ++iter)
    {
        ListItemBase* pItem = iter->first;

        UIASSERT(0 && "TODO: 未实现");
        pItem->DrawFloat(pRenderTarget, NULL);  
    }
}

ListItemBase*  FloatItemMgr::HitTest(POINT ptWindow)
{
    ListItemBase* pItem = NULL;
    long lZorder = 0x80000000;  //  最小值

    _mapiter iter = m_mapFloatItems.begin();
    for (; iter != m_mapFloatItems.end(); ++iter)
    {
        if (iter->second.zOrder <= lZorder)
            continue;
        
        if (PtInRect(&(iter->second.rcFloat), ptWindow))
        {
            pItem = iter->first;
            lZorder = iter->second.zOrder;
        }
    }

    return pItem;
}