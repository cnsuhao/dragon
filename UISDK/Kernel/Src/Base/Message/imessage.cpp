#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\imessage.h"
#include "UISDK\Kernel\Src\Base\Message\message.h"
#include "..\Object\object.h"

namespace UI
{

IMessage::IMessage()
{
    m_pMessageImpl = NULL;
    m_bCreateMessageImpl = FALSE;
}

IMessage::~IMessage()
{
    if (m_bCreateMessageImpl)
        SAFE_DELETE(m_pMessageImpl);
}

void  IMessage::CreateImpl(Message* p)
{
    if (p)
    {
        m_pMessageImpl = p;
    }
    else
    {
        m_pMessageImpl = new Message;
        m_bCreateMessageImpl = TRUE;
    }
    m_pMessageImpl->SetIMessage(this);
}

Message*  IMessage::GetImpl() 
{
    return m_pMessageImpl; 
}

// 用于UISDK内部直接继承，不需要Ixxx接口时使用，例如PopupControlWindow
void  IMessage::SetMessageImpl(Message* p) 
{
    m_pMessageImpl = p; 
}
 
BOOL  IMessage::ProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook) 
{
    UI::UIMSG*  pOldMsg  = this->GetCurMsg(); 
    BOOL bRet = virtualProcessMessage(pMsg, nMsgMapID, bDoHook);
    SetCurMsg(pOldMsg);
    return bRet;
}

void  IMessage::delete_this()
{
    this->do_delete_this();
}

BOOL  IMessage::virtualProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook)
{
    return FALSE;
}

BOOL  IMessage::IsMsgHandled()const
{
    return m_pMessageImpl->IsMsgHandled(); 
}
void  IMessage::SetMsgHandled(BOOL b) 
{
    m_pMessageImpl->SetMsgHandled(b); 
}
UIMSG*  IMessage::GetCurMsg() 
{
    return m_pMessageImpl->GetCurMsg(); 
}
void  IMessage::SetCurMsg(UIMSG* p) 
{
    m_pMessageImpl->SetCurMsg(p); 
}
BOOL  IMessage::DoHook(UIMSG* pMsg, int nMsgMapID) 
{ 
    return m_pMessageImpl->DoHook(pMsg, nMsgMapID); 
}

void  IMessage::ClearNotify()
{ 
    return m_pMessageImpl->ClearNotify(); 
}
void  IMessage::SetNotify(IMessage* pObj, int nMsgMapID) 
{ 
    return m_pMessageImpl->SetNotify(pObj, nMsgMapID); 
}
void  IMessage::CopyNotifyTo(IMessage* pObjCopyTo) 
{ 
    if (NULL == pObjCopyTo)
        return;

    return m_pMessageImpl->CopyNotifyTo(pObjCopyTo/*->GetMessageImpl()*/); 
}
long  IMessage::DoNotify(UIMSG* pMsg, bool bPost, IUIApplication* pUIApp) 
{
    return m_pMessageImpl->DoNotify(pMsg, bPost, pUIApp); 
}

IMessage*  IMessage::GetNotifyObj()
{
    return m_pMessageImpl->GetNotifyObj();
}

void  IMessage::AddHook(IMessage* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify ) 
{ 
    if (NULL == pObj)
        return;

    return m_pMessageImpl->AddHook(pObj/*->GetMessageImpl()*/, nMsgMapIDToHook, nMsgMapIDToNotify); 
}
void  IMessage::RemoveHook(IMessage* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify ) 
{ 
    if (NULL == pObj)
        return;

    return m_pMessageImpl->RemoveHook(pObj/*->GetMessageImpl()*/, nMsgMapIDToHook, nMsgMapIDToNotify); 
}
void  IMessage::RemoveHook(IMessage* pObj) 
{ 
    if (!pObj)
        return;

    return m_pMessageImpl->RemoveHook(pObj/*->GetMessageImpl()*/); 
}
void  IMessage::ClearHook() 
{ 
    return m_pMessageImpl->ClearHook(); 
}

void  IMessage::ForwardMessageToChildObject(UIMSG* pMsg)
{
	Message::ForwardMessageToChildObject(
		static_cast<Object*>(m_pMessageImpl), pMsg);
}
void  IMessage::ForwardMessageToChildObject2(UIMSG* pMsg, UIMSG* pMsg2)
{
	Message::ForwardMessageToChildObject2(
		static_cast<Object*>(m_pMessageImpl), pMsg, pMsg2);
}
void  IMessage::AddDelayRef(void** pp)
{
    return m_pMessageImpl->AddDelayRef(pp);
}
void  IMessage::RemoveDelayRef(void** pp)
{
    return m_pMessageImpl->RemoveDelayRef(pp);
}

void*  IMessage::QueryInterface(REFIID iid)
{
    return (void*)UISendMessage(this, UI_WM_QUERYINTERFACE, (WPARAM)&iid);
}
}