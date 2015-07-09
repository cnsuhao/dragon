#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\isplitterbar.h"
#include "UISDK\Control\Src\Control\SplitterBar\splitterbar.h"

using namespace UI;

UI_IMPLEMENT_Ixxx_INTERFACE_ACROSSMODULE(ISplitterBar, SplitterBar, IPanel);

void  ISplitterBar::UpdateControlPosBySplitterBarCurPos(bool bRedraw)
{
    m_pSplitterBarImpl->UpdateControlPosBySplitterBarCurPos(bRedraw);
}

int  ISplitterBar::GetLeftMin()
{
    return m_pSplitterBarImpl->GetLeftMin();
}
int  ISplitterBar::GetLeftMax()
{
    return m_pSplitterBarImpl->GetLeftMax();
}
int  ISplitterBar::GetRightMin()
{
    return m_pSplitterBarImpl->GetRightMin();
}
int  ISplitterBar::GetRightMax()
{
    return m_pSplitterBarImpl->GetRightMax();
}
int  ISplitterBar::GetUpMin()
{
    return m_pSplitterBarImpl->GetUpMin();
}
int  ISplitterBar::GetUpMax()
{
    return m_pSplitterBarImpl->GetUpMax();
}
int  ISplitterBar::GetDownMin()
{
    return m_pSplitterBarImpl->GetDownMin();
}
int  ISplitterBar::GetDownMax()
{
    return m_pSplitterBarImpl->GetDownMax();
}
