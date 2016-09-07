#pragma once
#include "UISDK\Project\UIEditorCtrl\Inc\iprojecttreeview.h"

namespace UI
{
class SkinTreeViewItem : public MessageProxy
{
public:
    SkinTreeViewItem();
    ~SkinTreeViewItem();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(SkinTreeViewItem, INormalTreeItem)

    void  SetISkinTreeViewItem(ISkinTreeViewItem* p)
    {
        m_pISkinTreeViewItem = p; 
        SetIMessageProxy(static_cast<IMessage*>(p));
    }

public:
    void  SetTextRender(ITextRenderBase**  pTextRender);
    void  OnPaint(IRenderTarget* pRenderTarget);

public:
    ISkinTreeViewItem*  m_pISkinTreeViewItem;
    ITextRenderBase**   m_ppTextRender;
};
}