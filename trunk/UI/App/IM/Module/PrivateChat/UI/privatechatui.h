#pragma once
#include "App\IM\include\framework_inc.h"
#include "App\IM\include\privatechat_inc.h"
class CChatDlgBase;
namespace IM
{
    struct ISkinUI;
}
namespace UI
{
    interface IButton;
}
class EmotionDlg;

class PrivateChatDlgInfo
{
public:
    PrivateChatDlgInfo();
    ~PrivateChatDlgInfo();

public:
    String  m_strUserId;
    CChatDlgBase*  m_pChatDlg;
};

class CPrivateChatUI : public IM::IUI
{
public:
    CPrivateChatUI();
    ~CPrivateChatUI();

    virtual long  ExecuteCommand(long nCommand, WPARAM wParam, LPARAM lParam);

    void  DelayDestroyDlg(CChatDlgBase* p);
    void  ShowEmotionDlg(HWND hWnd, POINT pt);

    long  OnOpenPrivateChatDlg(IM::OpenPrivateChatDlgReqTask* pTask);

protected:
    
    void  DestroyDlg(CChatDlgBase* p);

    PrivateChatDlgInfo*  FindDlg(const TCHAR* szUserId);
    PrivateChatDlgInfo*  FindDlg2(CChatDlgBase* pDlg);

public:
    vector<PrivateChatDlgInfo*>  m_vecPrivateChatDlg;
    IM::ISkinUI*  m_pSkinUI;
    EmotionDlg*   m_pEmotionDlg;
};