#ifndef IMESSAGE_H_3180BE93_DA9E_4c55_A49A_C1C79BEBC947
#define IMESSAGE_H_3180BE93_DA9E_4c55_A49A_C1C79BEBC947

namespace UI
{

struct UIMSG;
interface IUIApplication;
interface  IMessage;

// ��Ϣ�ṹ���塣ϵͳ�ؼ���Ȼʹ��MSG����UI�ؼ�ʹ��UIMsg���д���
struct UIMSG : public MSG
{
    UIMSG() { memset(this, 0, sizeof(UIMSG)); }

    IMessage*   pMsgFrom;    // ��Ϣ������
    IMessage*   pMsgTo;      // ��Ϣ������ 
    UINT        nCode;       // ��� WM_COMMAND,WM_NOTIFY
    long        lRet;        // ��Ϣ���������ķ���ֵ
    BOOL        bHandled;    // ����Ϣ�Ƿ��ѱ������
};

class Message;
interface UISDKAPI IMessage
{
    IMessage();
    BOOL  ProcessMessage(UIMSG* pMsg, int nMsgMapID=0, bool bDoHook=false);
    void  delete_this();

protected:
    friend class Message;  
    virtual ~IMessage();  // �麯��1. ��֤��ȷ�ͷ���������
private:
    virtual BOOL  virtualProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook);  // �麯��2. ��Ϣ����
    virtual void  do_delete_this(){ }  // ��UIObjCreator����ʵ��

public:
    void    CreateImpl(Message* p = NULL);
    Message* GetImpl();
    void    SetMessageImpl(Message* p);

    BOOL    IsMsgHandled()const;
    void    SetMsgHandled(BOOL b);

    UIMSG*  GetCurMsg();
    void  SetCurMsg(UIMSG* p);
    BOOL  DoHook(UIMSG* pMsg, int nMsgMapID);
	void  ForwardMessageToChildObject(UIMSG* pMsg);
	void  ForwardMessageToChildObject2(UIMSG* pMsg, UIMSG* pMsg2);

    void  ClearNotify();
    void  SetNotify(IMessage* pObj, int nMsgMapID);
    void  CopyNotifyTo(IMessage* pObjCopyTo);
    long  DoNotify(UIMSG* pMsg, bool bPost=false, IUIApplication* pUIApp=NULL);
    IMessage*  GetNotifyObj();

    void  AddHook(IMessage* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify );
    void  RemoveHook(IMessage* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify );
    void  RemoveHook(IMessage* pObj );
    void  ClearHook();

    void  AddDelayRef(void** pp);
    void  RemoveDelayRef(void** pp);

    void*  QueryInterface(REFIID iid);

protected:
    Message*   m_pMessageImpl;
    BOOL       m_bCreateMessageImpl;
};

// ���ⲿ����̳У�����֧����Ϣӳ��궨��
class MessageProxy
{
public:
    MessageProxy() { m_pIMessageProxy = NULL; }
    virtual ~MessageProxy() {}

    void    SetIMessageProxy(IMessage* p) { m_pIMessageProxy = p; }

    BOOL    IsMsgHandled()const   { return m_pIMessageProxy->IsMsgHandled(); }
    void    SetMsgHandled(BOOL b) { return m_pIMessageProxy->SetMsgHandled(b); }
    UIMSG*  GetCurMsg()           { return m_pIMessageProxy->GetCurMsg(); }
    void    SetCurMsg(UIMSG* p)   { m_pIMessageProxy->SetCurMsg(p); }
    BOOL    DoHook(UIMSG* pMsg, int nMsgMapID) { return m_pIMessageProxy->DoHook(pMsg, nMsgMapID); }

protected:
    IMessage*  m_pIMessageProxy;
};

#define DO_PARENT_PROCESS(IMyInterface, IParentInterface) \
    static_cast<IParentInterface*>(m_p##IMyInterface)->nvProcessMessage(GetCurMsg(), 0, 0); \
    SetMsgHandled(TRUE)

// ��ȡ�����Ƿ���
#define DO_PARENT_PROCESS2(IMyInterface, IParentInterface, bHandled) \
	bHandled = static_cast<IParentInterface*>(m_p##IMyInterface)->nvProcessMessage(GetCurMsg(), 0, 0); \
	SetMsgHandled(TRUE)
    
// ��ȡ����ֵ
#define DO_PARENT_PROCESS3(IMyInterface, IParentInterface) \
	(static_cast<IParentInterface*>(m_p##IMyInterface)->nvProcessMessage(GetCurMsg(), 0, 0), \
	SetMsgHandled(TRUE), GetCurMsg()->lRet);

#define DO_PARENT_PROCESS_MAPID(IMyInterface, IParentInterface, MsgMapId) \
    static_cast<IParentInterface*>(m_p##IMyInterface)->nvProcessMessage(GetCurMsg(), MsgMapId, 0); \
    SetMsgHandled(TRUE)

}

#endif  IMESSAGE_H_3180BE93_DA9E_4c55_A49A_C1C79BEBC947