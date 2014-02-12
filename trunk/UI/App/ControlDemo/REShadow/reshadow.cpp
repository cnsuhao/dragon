#include "StdAfx.h"
#include "REShadow.h"
#include "UISDK\Control\Inc\Interface\irichedit.h"

REShadowWindow::REShadowWindow()
{
    m_pRichEdit = NULL;
}

REShadowWindow::~REShadowWindow(void)
{

}

void REShadowWindow::OnInitWindow()
{
    __super::nvProcessMessage(GetCurMsg(), 0, 0);

    m_pRichEdit = (UI::IRichEdit*)this->FindChildObject(_T("richedit"));
    if (m_pRichEdit)
    {
        //m_pRichEdit->SetMultiLine(false);
        m_pRichEdit->ModifyStyleEx(RICHEDIT_STYLE_TEXTSHADOW, 0, true);

        CHARFORMAT2 cf;
        memset(&cf, 0, sizeof(cf));
        cf.cbSize = sizeof(cf);
        cf.dwMask = CFM_COLOR;
        cf.crTextColor = RGB(255,255,255);
        m_pRichEdit->SetCharFormat(&cf);
    }
}
