#include "stdafx.h"
#include "skintreeviewitem.h"

namespace UI
{
SkinTreeViewItem::SkinTreeViewItem()
{
    m_pISkinTreeViewItem = NULL;
}
SkinTreeViewItem::~SkinTreeViewItem()
{

}

void  SkinTreeViewItem::SetTextRender(ITextRenderBase**  pTextRender)
{
    m_ppTextRender = pTextRender;
}

void  SkinTreeViewItem::OnPaint(IRenderTarget* pRenderTarget)
{
    if (!m_ppTextRender || !*m_ppTextRender)
    {
        SetMsgHandled(FALSE);
        return;
    }

    // Ìæ»»×ÖÌå
    ITextRenderBase*  pTextRender = *m_ppTextRender;
    IListCtrlBase*  pListCtrlBase = m_pISkinTreeViewItem->GetIListCtrlBase();
    ITextRenderBase*  pOldTextRender = pListCtrlBase->GetTextRender();
    if (pOldTextRender)
        pOldTextRender->AddRef();

    pListCtrlBase->SetTextRender(pTextRender);
    DO_PARENT_PROCESS(ISkinTreeViewItem, INormalTreeItem);
    pListCtrlBase->SetTextRender(pOldTextRender);

    SAFE_RELEASE(pOldTextRender);
    
}

}