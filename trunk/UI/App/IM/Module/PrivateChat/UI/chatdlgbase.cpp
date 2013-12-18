#include "stdafx.h"
#include "chatdlgbase.h"

CChatDlgBase::CChatDlgBase(void)
{
	m_pPrivateChatUI = NULL;
}
CChatDlgBase::~CChatDlgBase(void)
{

}

void  CChatDlgBase::SetPrivateChatUI(CPrivateChatUI* p)
{
	m_pPrivateChatUI = p;
}