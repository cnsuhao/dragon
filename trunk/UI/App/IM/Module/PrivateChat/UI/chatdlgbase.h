#pragma once
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
#include "emotiondlg.h"

class CPrivateChatUI;

class CChatDlgBase : public UI::ICustomWindow
{
public:
	CChatDlgBase(void);
	virtual ~CChatDlgBase(void);

	UI_BEGIN_MSG_MAP_Ixxx(CChatDlgBase)
	UI_END_MSG_MAP_CHAIN_PARENT(UI::ICustomWindow)

	void  SetPrivateChatUI(CPrivateChatUI* p);

	virtual void  SetSkin(UI::IRenderBase* p){};

protected:
	CPrivateChatUI*  m_pPrivateChatUI;
};