#include "stdafx.h"
#ifdef COMPILE_Win32Edit
#include "UISDK\Control\Inc\Interface\iwin32edit.h"
#include "UISDK\Control\Src\Control\Win32Edit\win32edit.h"

using namespace UI;

UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IWin32Edit, Win32Edit, IControl)

void  IWin32Edit::SetText(LPCTSTR szText, bool bUpdateNow)
{
    m_pWin32EditImpl->SetText(szText, bUpdateNow);
}

bool  IWin32Edit::StartEdit(bool bSelAll)
{
    return m_pWin32EditImpl->StartEdit(bSelAll) ? true:false;
}

void  IWin32Edit::SetNotifyStartEdit(bool b)
{
    m_pWin32EditImpl->SetNotifyStartEdit(b);
}
void  IWin32Edit::SetNotifyAcceptEdit(bool b)
{
    m_pWin32EditImpl->SetNotifyAcceptEdit(b);
}
void  IWin32Edit::SetNotifyCancelEdit(bool b)
{
    m_pWin32EditImpl->SetNotifyCancelEdit(b);
}
void  IWin32Edit::SetNotifyDestroyEdit(bool b)
{
    m_pWin32EditImpl->SetNotifyDestroyEdit(b);
}
bool  IWin32Edit::IsEditing()
{
    return m_pWin32EditImpl->IsEditing();
}
void  IWin32Edit::DiscardEditNow()
{
    m_pWin32EditImpl->DiscardEditNow();
}
#endif