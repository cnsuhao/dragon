#pragma once
#include "..\model\node.h"
#include "Inc\Interface\irichtext.h"

namespace UI
{
    namespace RT{

class ChatMessageHead : public Node
{
public:
    virtual NodeType GetNodeType() override {
        return NodeType::ChatMessageHead;
    }
};

class ChatMessagePortrait : public Node
{
public:
    virtual NodeType GetNodeType() override {
        return NodeType::ChatMessagePortrait;
    }
};

class ChatMessage;
class ChatMessageBubble : public Node
{
public:
    ChatMessageBubble(ChatMessage& message) : m_message(message){

    }
    virtual NodeType GetNodeType() override {
        return NodeType::ChatMessageBubble;
    }

    virtual void PreDraw(HDC hDC) override;
    virtual void PostLayoutMultiLine(MultiLineLayoutContext*) override;
    virtual void PreLayoutMultiLine(MultiLineLayoutContext* context) override;

private:
    ChatMessage& m_message;

    CRect  m_rcBubble;
    int  m_nTopIndent = 10;
    int  m_nBottomIndent = 10;
};

class ChatMessage : public Node, public IChatMessage
{
public:
    ChatMessage(Doc&  doc);

    virtual NodeType GetNodeType() override { return NodeType::ChatMessage; }
    virtual void PreDraw(HDC hDC) override;
    virtual void PostLayoutMultiLine(MultiLineLayoutContext*) override;
    virtual void PreLayoutMultiLine(MultiLineLayoutContext* context) override;

    virtual void  SetAlign(RT::HALIGN align) override;
    virtual HANDLE  EnterBubble() override;
    virtual void  LeaveBubble(HANDLE) override;

    CRegion4  GetMessageIndent();

    
private:
    bool  m_bDrawBubbleBackground = true;
    // 第一行作为消息头，显示用户昵称，不放在气泡内
    bool  m_bFirstLineAsHead = true;
    CRect  m_rcPortrait;

    // 气泡内容的缩进，用于为气泡让点位置
    int  m_nLeftIndent = 0;
    int  m_nRightIndent = 0;
    int  m_nTopIndent = 0;
    int  m_nBottomIndent = 0;
public:
    Doc&  m_doc;
    RT::HALIGN  m_hAlign;

//     ChatMessageHead*  m_pHead = nullptr;
//     ChatMessagePortrait*  m_pPortrait = nullptr;
//     ChatMessageBubble*  m_pContent = nullptr;
};



    }
}