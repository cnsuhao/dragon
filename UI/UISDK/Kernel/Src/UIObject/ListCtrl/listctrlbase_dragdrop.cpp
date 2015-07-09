#include "stdafx.h"
#include "listctrlbase.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"

using namespace UI;

// ��ק�����У����������б�����ڻ��Ƹ���״̬
// Ĭ��ͬһʱ��ֻ��һ����ק����
// Ĭ��ÿ����ק����g_pItemUnderDrag���ָ�ΪNULL
IListItemBase*  g_pItemUnderDrag = NULL;  

void  ListCtrlBase::ListItemDragDropEvent(
    UI::DROPTARGETEVENT_TYPE eEvent, IListItemBase* pItem)
{
    UIMSG  msg;
    msg.message = UI_LISTITEM_MSG_DRAGDROP_EVENT;
    
    if (eEvent == UI::_DragLeave)
    {
        if (g_pItemUnderDrag)
        {
            g_pItemUnderDrag->SetDragDropHover(false);
            this->InvalidateItem(g_pItemUnderDrag->GetImpl());

            msg.pMsgTo = g_pItemUnderDrag;
            msg.wParam = (WPARAM)eEvent;
            UISendMessage(&msg);
            g_pItemUnderDrag = NULL;

            this->Refresh();
        }
        return;
    }

    if (g_pItemUnderDrag != pItem && g_pItemUnderDrag)
    {
        g_pItemUnderDrag->SetDragDropHover(false);
        this->InvalidateItem(g_pItemUnderDrag->GetImpl());

        msg.pMsgTo = g_pItemUnderDrag;
        msg.wParam = UI::_DragLeave;
        UISendMessage(&msg);

    }

    if (g_pItemUnderDrag)
        g_pItemUnderDrag->RemoveDelayRef((void**)&g_pItemUnderDrag);

    g_pItemUnderDrag = pItem;

    if (g_pItemUnderDrag)
        g_pItemUnderDrag->AddDelayRef((void**)&g_pItemUnderDrag);

    if (g_pItemUnderDrag)
    {
        if (eEvent == UI::_Drop)
            g_pItemUnderDrag->SetDragDropHover(false);
        else
            g_pItemUnderDrag->SetDragDropHover(true);

        this->InvalidateItem(g_pItemUnderDrag->GetImpl());

        msg.pMsgTo = g_pItemUnderDrag;
        msg.wParam = eEvent;
        UISendMessage(&msg);
    }

    this->Refresh();
}

// ��ק�����У���ק���������ߵײ�ʱ�������б�
void  ListCtrlBase::ListCtrlDragScroll()
{
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(m_pIListCtrlBase->GetHWND(), &pt);

    POINT ptObject;
    m_pIListCtrlBase->WindowPoint2ObjectPoint(&pt, &ptObject, true);

    CRect rcClient;
    m_pIListCtrlBase->GetClientRectInObject(&rcClient);
    rcClient.DeflateRect(10,10,10,10);

    bool bNeedScroll = true;
    if (ptObject.y < rcClient.top)
    {
        this->ScrollY(-10, true);
    }
    else if (ptObject.y > rcClient.bottom)
    {
        this->ScrollY(10, true);
    }
    else
    {
        bNeedScroll = false;
    }
}