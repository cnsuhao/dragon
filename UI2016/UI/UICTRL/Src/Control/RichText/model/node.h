#pragma once
#include "selection.h"

namespace UI {
    struct CharFormat;

    namespace RT {

class Doc;
class Paragraph;
struct SingleLineLayoutContext;
struct MultiLineLayoutContext;

struct NodeStyle
{
    bool hover : 1;
    bool press : 1;
};
enum class NodeType
{
    Unknown,
    VirtualNodeBegin = 100,
        Doc,
        Paragraph,
        Link,
        ChatMessage,
        ChatMessageHead,
        ChatMessagePortrait,
        ChatMessageBubble,
    VirtualNodeEnd,

    RealNodeBegin = 200,
        Text,
        Image,
        Object,
    RealNodeEnd,
};

class Node
{
public:
    Node();
    virtual ~Node() {}
    virtual NodeType  GetNodeType() = 0;

    Node*  GetParentNode() { return parent; }
    Node*  GetChildNode() { return child; }
    Node*  GetPrevNode() { return prev; }
    Node*  GetNextNode() { return next; }

    Node*  GetLastChildNode();
    Node*  GetPrevTreeNode();
    Node*  GetNextTreeNode();

    Node*  GetRootNode();
    Paragraph*  GetParagraphNode();
    Node* GetAncestorNodeByType(NodeType);
    Doc*  GetDocNode();
    
    void  AddChildNode(Node*);
	void  InsertAfter(Node* pInsertAfter);
    void  TakePlaceAsMyParent(Node* pNewParent);

    bool IsVirtualNode();
    bool IsRealNode();

    void  SetSelectionState(SelectionState e);
    SelectionState  GetSelectionState();
    void  SetIndex(int);
    int   GetIndex();


    virtual void PreLayoutSingleLine(SingleLineLayoutContext*);
    virtual void LayoutSingleLine(SingleLineLayoutContext*);
    virtual void PostLayoutSingleLine(SingleLineLayoutContext*);

    virtual void PreLayoutMultiLine(MultiLineLayoutContext*);
    virtual void LayoutMultiLine(MultiLineLayoutContext*);
    virtual void PostLayoutMultiLine(MultiLineLayoutContext*);

    virtual void PreDraw(HDC hDC);
    virtual BOOL OnSetCursor();
    virtual void OnLButtonDown(UINT nFlags, POINT ptDoc);
    virtual void OnLButtonUp(UINT nFlags, POINT ptDoc);
    virtual void OnClick(UINT nFlags, POINT ptDoc);

    virtual void OnMouseEnter() {};
    virtual void OnMouseLeave() {};
    virtual void OnMouseMove(UINT nFlags, POINT ptDoc) {}

    void  BubbleMouseLeave(Node* pBubbleUntil);
    void  BubbleMouseMove(UINT nFlags, POINT ptDoc, Node* pBubbleUntil);

    virtual const CharFormat*  GetCharFormat();
    virtual uint  GetCharacters() { return 0; }
    virtual void  ToString(
            CaretPos start,
            CaretPos end,
            int format,
            String& out) {};

private:
    Node*  parent = nullptr;
    Node*  child = nullptr;
    Node*  prev = nullptr;
    Node*  next = nullptr;

    NodeStyle  m_style;

    SelectionState  m_eSelectionState = SelectionState::None;
    int  m_nIndex = 0;  // 在doc中的索引，用于element之间的前后顺序比较
};

}
}