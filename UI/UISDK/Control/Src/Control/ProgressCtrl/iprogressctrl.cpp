#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\iprogressctrl.h"
#include "UISDK\Control\Src\Control\ProgressCtrl\progressctrl.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(IProgressBar, ProgressBar, IControl)

int   IProgressBar::SetPos(int nPos, bool bUpdate) 
{
    return m_pProgressBarImpl->SetPos(nPos, bUpdate); 
}
int   IProgressBar::SetPage(int nPage, bool bUpdate)
{
    return m_pProgressBarImpl->SetPage(nPage, bUpdate); 
}
void  IProgressBar::SetRange(int nLower, int nUpper, bool bUpdate)
{ 
    m_pProgressBarImpl->SetRange(nLower, nUpper, bUpdate);
}
void  IProgressBar::SetScrollInfo(LPCSCROLLINFO lpsi, bool bUpdate)
{
    m_pProgressBarImpl->SetScrollInfo(lpsi, bUpdate);
}

void  IProgressBar::SetLine(int nLine) 
{ 
    m_pProgressBarImpl->SetLine(nLine);
}
int   IProgressBar::GetPos() 
{ 
    return m_pProgressBarImpl->GetPos();
}
int   IProgressBar::GetPage()
{
    return m_pProgressBarImpl->GetPage();
}
int   IProgressBar::GetLine()
{ return m_pProgressBarImpl->GetLine(); 
}
void  IProgressBar::GetRange(int& nLower, int& nUpper)
{ 
    m_pProgressBarImpl->GetRange(nLower, nUpper); 
}
int   IProgressBar::GetRange() 
{ 
    return m_pProgressBarImpl->GetRange(); 
}
void  IProgressBar::SetDirectionType(PROGRESSBAR_SCROLL_DIRECTION_TYPE eType)
{
    m_pProgressBarImpl->SetDirectionType(eType); 
}
PROGRESSBAR_SCROLL_DIRECTION_TYPE  IProgressBar::GetDirectionType() 
{
    return m_pProgressBarImpl->GetDirectionType();
}

}