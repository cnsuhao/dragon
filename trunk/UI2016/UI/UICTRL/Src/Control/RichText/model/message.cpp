#include "stdafx.h"
#include "message.h"
#include "line.h"
#include "layout_context.h"
#include "doc.h"
#include "paragraph.h"
#include "..\UISDK\Inc\Util\struct.h"

namespace UI {
namespace RT {

ChatMessage::ChatMessage(Doc&  doc) : m_doc(doc)
{
    SetAlign(RT::HALIGN::left);
}

void ChatMessage::SetAlign(RT::HALIGN align)
{
    m_hAlign = align;

    m_nLeftIndent = 0;
    m_nRightIndent = 0;
    m_nTopIndent = 0;
    m_nBottomIndent = 0;

    switch (m_hAlign)
    {
    case RT::HALIGN::left:
    {
        m_nLeftIndent = 50;
        m_nRightIndent = 100;
    }
    break;

    case RT::HALIGN::right:
    {
        m_nLeftIndent = 100;
        m_nRightIndent = 50;
    }
    break;

    case RT::HALIGN::center:
    {
        m_nLeftIndent = 50;
        m_nRightIndent = 50;
    }
    break;
    }
}

HANDLE  ChatMessage::EnterBubble()
{
    Doc* pDoc = GetDocNode();
    UIASSERT(pDoc);
    if (!pDoc)
        return nullptr;

    ChatMessageBubble* bubble = new ChatMessageBubble(*this);
    AddChildNode(bubble);
    
    Selection& s = pDoc->GetSelection();
    s.SetAnchorFocus(bubble, CaretPos(0));

    return (HANDLE)bubble;
}

void  ChatMessage::LeaveBubble(HANDLE handle)
{
    ChatMessageBubble* bubble = (ChatMessageBubble*)handle;
    Doc* pDoc = GetDocNode();
    UIASSERT(pDoc);
    if (!pDoc)
        return;

    Selection& s = pDoc->GetSelection();
    s.SetAnchorFocus(bubble, -1);
}

void ChatMessage::PreDraw(HDC hDC)
{
    FillRect(hDC, &m_rcPortrait, (HBRUSH)GetStockObject(GRAY_BRUSH));

    Node::PreDraw(hDC);
}

void ChatMessage::PreLayoutMultiLine(MultiLineLayoutContext* context)
{
    context->yLine += m_nTopIndent;
    context->nLeftIndent += m_nLeftIndent;
    context->nRightIndent += m_nRightIndent;

    // TODO: 在这里设置每个段落的对齐方式？？解决消息头对齐方式与message不统一的问题
    Node* p = GetChildNode();
    while (p)
    {
        if (p->GetNodeType() == NodeType::Paragraph)
        {
            switch (m_hAlign)
            {
            case HALIGN::right:
                static_cast<Paragraph*>(p)->SetAlign(HALIGN::paraRight);
                break;

            case HALIGN::center:
                static_cast<Paragraph*>(p)->SetAlign(HALIGN::paraCenter);
                break;

            default:
                static_cast<Paragraph*>(p)->SetAlign(HALIGN::left);
                break;
            }            
        }
        p = p->GetNextNode();
    }
}

//
// message参数布局的情况下，将禁用Paragraph的布局功能。参见Paragraph的布局逻辑
//
void ChatMessage::PostLayoutMultiLine(MultiLineLayoutContext* context)
{
    m_rcPortrait.SetRect(0, 0, 40, 40);

    Line* pFirstLine = context->pLine;
    Line* pLine = context->pLine;
    while (pLine)
    {
        if (pLine->GetChatMessage() != this)
            break;

        pFirstLine = pLine;
        pLine = pLine->GetPrevLine();
    }
    m_rcPortrait.OffsetRect(0, pFirstLine->GetPositionYInPage());


    // 找到第一行，用于设置头像的纵向起始位置
    if (m_hAlign == RT::HALIGN::right)
    {
        m_rcPortrait.OffsetRect(context->nPageContentWidth - context->nRightIndent + 10, 0);
    }

    context->yLine += m_nBottomIndent;
    context->nLeftIndent -= m_nLeftIndent;
    context->nRightIndent -= m_nRightIndent;
}


void ChatMessageBubble::PreDraw(HDC hDC)
{
//     if (!m_bDrawBubbleBackground)
//         return;

#if 0
    FillRect(hDC, &m_rcBubble, (HBRUSH)GetStockObject(GRAY_BRUSH));
#else
    ImageWrap* image = nullptr;
    HALIGN hAlign = m_message.m_hAlign;
    if (hAlign == RT::HALIGN::left)
    {
        image = &m_message.m_doc.m_imageLeftBubble;
    }
    else if (hAlign == RT::HALIGN::right)
    {
        image = &m_message.m_doc.m_imageRightBubble;
    }

    if (image)
    {
        C9Region r;
        r.Set(15);

        CRect rcDraw = m_rcBubble;
        rcDraw.InflateRect(10, 0);

        image->Draw(hDC,
            rcDraw.left, rcDraw.top,
            rcDraw.Width(), rcDraw.Height(),
            0, 0,
            image->GetWidth(), image->GetHeight(),
            &r);
    }
#endif
}

void ChatMessageBubble::PreLayoutMultiLine(MultiLineLayoutContext* context)
{
    context->yLine += m_nTopIndent;
}
void ChatMessageBubble::PostLayoutMultiLine(MultiLineLayoutContext* context)
{
    SetRectEmpty(&m_rcBubble);

    // 获取到气泡内容首行

    Line* pLastLine = context->pLine;
    Line* pFirstLine = pLastLine;

    int nLineMaxWidth = 0;

    Line* pLine = context->pLine;
    UIASSERT(pLine);
    while (pLine)
    {
        if (pLine->GetChatMessageBubble() != this)
            break;

        int width = pLine->GetRunsWidth();
        if (width > nLineMaxWidth)
            nLineMaxWidth = width;

        pFirstLine = pLine;
        pLine = pLine->GetPrevLine();
    }

    m_rcBubble.left = 0;
    m_rcBubble.right = nLineMaxWidth;
    m_rcBubble.bottom =
        pLastLine->GetPositionYInPage() + pLastLine->GetHeight();
    m_rcBubble.top = pFirstLine->GetPositionYInPage();

    m_rcBubble.top -= m_nTopIndent;
    m_rcBubble.bottom += m_nBottomIndent;

    Doc* pDoc = GetDocNode();
    if (pDoc)
    {
        m_rcBubble.OffsetRect(
            pDoc->GetStyle().page_padding.left,
            pDoc->GetStyle().page_padding.top);
    }
    int leftIndex = 0;
    HALIGN hAlign = m_message.m_hAlign;
    if (hAlign == RT::HALIGN::left)
    {
        leftIndex = context->nLeftIndent;
    }
    else if (hAlign == RT::HALIGN::right)
    {
        leftIndex =
            context->nPageContentWidth -
            nLineMaxWidth -
            context->nRightIndent;
    }
    else if (hAlign == RT::HALIGN::center)
    {
        leftIndex = context->nLeftIndent +
            (context->nPageContentWidth -
            nLineMaxWidth -
            context->nLeftIndent -
            context->nRightIndent) / 2;
    }

    pLine = pFirstLine;
    while (1)
    {
        // 横向每一行自己调整
        pLine->SetPositionXInPage(leftIndex);
        if (pLine == pLastLine)
            break;

        pLine = pLine->GetNextLine();
    }

    m_rcBubble.OffsetRect(leftIndex, 0);

    context->yLine += m_nBottomIndent;
}

}}

