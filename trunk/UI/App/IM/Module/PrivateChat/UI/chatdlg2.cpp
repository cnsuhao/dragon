#include "StdAfx.h"
#include "ChatDlg2.h"
#include "UISDK\Control\Inc\Interface\isliderctrl.h"
#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include "UISDK\Control\Inc\Interface\irichedit.h"
#include "privatechatui.h"
#include "UISDK\Control\Inc\Interface\isplitterbar.h"
#include "UISDK\Control\Inc\Interface\icheckbutton.h"
#include "UISDK\Control\Inc\Interface\icombobox.h"
#include "UISDK\Kernel\Inc\Util\ibuffer.h"
#include "UISDK\Control\Inc\Interface\ilistbox.h"


CChatDlg2::CChatDlg2(void)
{
    m_pSplitterBar = NULL;
    m_pBtnFont = NULL;
    m_pBtnEmotion = NULL;
    m_pBtnClose = NULL;
    m_pBtnSend = NULL;
    m_pBtnSendMode = NULL;

    m_pRichEditInput = m_pRichEditOutput = NULL;
    m_pPanelFontToolbar = NULL;
    m_pBtnFontBold = NULL;
    m_pBtnFontItalic = NULL;
    m_pBtnFontUnderline = NULL;
    m_pComboFontSize = NULL;
    m_pComboFontFace = NULL;
    m_pBtnFontColor = NULL;
}

CChatDlg2::~CChatDlg2(void)
{

}

void CChatDlg2::OnInitWindow()
{
    ICustomWindow::nvProcessMessage(GetCurMsg(), 0, 0);
    this->SetWindowResizeType(WRSB_ALL);

    m_pSplitterBar = (UI::ISplitterBar*)this->FindChildObject(_T("splitterbar"));
    m_pPanelFontToolbar = (UI::IPanel*)this->FindChildObject(_T("panel_font"));

    if (m_pSplitterBar)
    {
        m_pBtnFont = (UI::ICheckButton*)m_pSplitterBar->FindChildObject(_T("toolbar_btn_font"));
        m_pBtnEmotion = (UI::IButton*)m_pSplitterBar->FindChildObject(_T("toolbar_btn_face"));
        if (m_pBtnEmotion)
        {
            m_pBtnEmotion->ModifyStyleEx(BUTTON_STYLE_CLICK_ONDOWN, 0, false);
        }
    }

        m_pRichEditOutput = (UI::IRichEdit*)this->FindChildObject(_T("outputedit"));
        m_pRichEditInput = (UI::IRichEdit*)this->FindChildObject(_T("inputedit"));
        m_pBtnClose = (UI::IButton*)this->FindChildObject(_T("btnclose"));
        m_pBtnSend = (UI::IButton*)this->FindChildObject(_T("btnsend"));
        m_pBtnSendMode = (UI::IButton*)this->FindChildObject(_T("btnsendmode"));

    if (m_pPanelFontToolbar)
    {
        m_pBtnFontBold = (UI::ICheckButton*)m_pPanelFontToolbar->FindChildObject(_T("toolbar_btn_font_bold"));
        m_pBtnFontItalic = (UI::ICheckButton*)m_pPanelFontToolbar->FindChildObject(_T("toolbar_btn_font_italic"));
        m_pBtnFontUnderline = (UI::ICheckButton*)m_pPanelFontToolbar->FindChildObject(_T("toolbar_btn_font_underline"));
        m_pComboFontSize = (UI::IComboBox*)m_pPanelFontToolbar->FindChildObject(_T("toolbar_combo_font_size"));
        m_pComboFontFace = (UI::IComboBox*)m_pPanelFontToolbar->FindChildObject(_T("toolbar_combo_font_face"));
        m_pBtnFontColor = (UI::IButton*)m_pPanelFontToolbar->FindChildObject(_T("toolbar_btn_font_color"));
    }

    if (m_pRichEditOutput)
    {
        m_pRichEditOutput->SetReadOnly(true);
    }
    if (m_pRichEditInput)
    {
    }

    if (m_pComboFontSize)
    {
        TCHAR szText[4];
        for (int i = 8; i <= 22; i++)
        {
            _stprintf(szText, _T("%d"), i);
            m_pComboFontSize->AddStringEx(szText)->SetData(i);
        }
        if (m_pRichEditInput)
        {
            CHARFORMAT2 cf;
            memset(&cf, 0, sizeof(cf));
            cf.dwMask = CFM_SIZE|CFM_FACE;
            m_pRichEditInput->GetCharFormat(&cf);

            HWND hWnd = GetDesktopWindow();
            HDC  hDC = GetDC(hWnd);
            LONG yPixPerInch = GetDeviceCaps(hDC, LOGPIXELSY);
            int nSize = abs(UI::Util::FontHeight2Size(cf.yHeight * yPixPerInch / 1440));
            ReleaseDC(hWnd, hDC);

            m_pComboFontSize->SetCurSel(nSize-8);
            if (m_pComboFontFace)
            {
                m_pComboFontFace->AddStringEx(cf.szFaceName);
                m_pComboFontFace->SetCurSel(0);
            }
        }
    }
}

void  CChatDlg2::SetSkin(UI::IRenderBase* p)
{
    if (NULL == this->GetTextureRender())
    {
        this->SetTextureRender(p);
    }
}

void  CChatDlg2::OnTextureAlphaChanged(int nNewAlpha)
{
//	if (nScrollType == SB_ENDSCROLL)
//	{
//         UI::IImageManager* pImageMgr = NULL;
// 		g_pUIApp->GetActiveSkinImageMgr(&pImageMgr);
// 		if (pImageMgr)
// 			pImageMgr->ModifyImageItemAlpha(_T("chatframe.png"), nPos);
// 
// 		g_pUIApp->RedrawTopWindows();
//	}


#if 0
    UI::IRenderBase* p = this->GetForeRender();
    if (NULL == p)
        return;

    UI::IImageRender*  pBkgndRender = (UI::IImageRender*)p->QueryInterface(UI::uiiidof(IImageRender));
    if (NULL == pBkgndRender)
        return;

    pBkgndRender->SetAlpha(nNewAlpha);
#endif
}

void CChatDlg2::OnSysCommand(UINT nID, CPoint point)
{
	SetMsgHandled(FALSE);
}
void  CChatDlg2::OnClose()
{
    SetMsgHandled(TRUE);
    m_pPrivateChatUI->DelayDestroyDlg(this);
}


void  CChatDlg2::OnBtnFont()
{
    if (!m_pPanelFontToolbar)
        return;

    m_pPanelFontToolbar->SetVisible(!m_pPanelFontToolbar->IsMySelfVisible(), true, true);
}
void  CChatDlg2::OnBtnFontBold()
{
    if (!m_pRichEditInput)
        return;

    CHARFORMAT2 cf;
    memset(&cf, 0, sizeof(cf));
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_BOLD;

    if (m_pBtnFontBold->IsChecked())
        cf.dwEffects |= CFE_BOLD;
    else
        cf.dwEffects = 0;

    m_pRichEditInput->SetCharFormat(&cf);
}
void  CChatDlg2::OnBtnFontItalic()
{
    if (!m_pRichEditInput)
        return;

    CHARFORMAT2 cf;
    memset(&cf, 0, sizeof(cf));
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_ITALIC;

    if (m_pBtnFontItalic->IsChecked())
        cf.dwEffects |= CFE_ITALIC;
    else
        cf.dwEffects = 0;

    m_pRichEditInput->SetCharFormat(&cf);
}
void  CChatDlg2::OnBtnFontUnderline()
{
    if (!m_pRichEditInput)
        return;

    CHARFORMAT2 cf;
    memset(&cf, 0, sizeof(cf));
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_UNDERLINE;

    if (m_pBtnFontUnderline->IsChecked())
        cf.dwEffects |= CFE_UNDERLINE;
    else
        cf.dwEffects = 0;

    m_pRichEditInput->SetCharFormat(&cf);
}

void  CChatDlg2::OnBtnFontColor()
{
    if (!m_pRichEditInput)
        return;

    static COLORREF acrCustClr[16] = {0x00FFFFFF}; // array of custom colors
    static DWORD rgbCurrent = 0;        // initial color selection

    CHOOSECOLOR cc = {0};
    cc.lStructSize = sizeof(cc);
    cc.hwndOwner = GetHWND();
    cc.lpCustColors = (LPDWORD) acrCustClr;
    cc.rgbResult = rgbCurrent;
    cc.rgbResult = 0;
    cc.Flags = CC_FULLOPEN;

    if (!ChooseColor(&cc)) 
       return;

    CHARFORMAT2 cf;
    memset(&cf, 0, sizeof(cf));
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_COLOR;
    cf.crTextColor = cc.rgbResult;

    m_pRichEditInput->SetCharFormat(&cf);
}

void  CChatDlg2::OnComboSizeSelChanged(UI::IListItemBase* pOldItem, UI::IListItemBase* pNewItem)
{
    if (!pNewItem)
        return;

    int nSize = (int)pNewItem->GetData();   
    if (!m_pRichEditInput)
        return;

    CHARFORMAT2 cf;
    memset(&cf, 0, sizeof(cf));
    cf.cbSize = sizeof(cf);
    cf.dwMask = CFM_SIZE;

    HWND hWnd = GetDesktopWindow();
    HDC  hDC = GetDC(hWnd);
    LONG yPixPerInch = GetDeviceCaps(hDC, LOGPIXELSY);
    cf.yHeight = abs(UI::Util::FontSize2Height(nSize) * 1440 / yPixPerInch);
    ReleaseDC(hWnd, hDC);

    m_pRichEditInput->SetCharFormat(&cf);
}
void  CChatDlg2::OnComboFontFaceSelChanged(UI::IListItemBase* pOldItem, UI::IListItemBase* pNewItem)
{

}

void  CChatDlg2::OnBtnEmotion()
{
    if (m_pBtnEmotion->IsForcePress())
        return;

    POINT pt = m_pBtnEmotion->GetRealPosInWindow();
    ::ClientToScreen(GetHWND(), &pt);
    pt.x -= 4;
    m_pPrivateChatUI->ShowEmotionDlg(GetHWND(), pt);
    m_pBtnEmotion->SetForcePress(true, false);
}
LRESULT  CChatDlg2::OnEmotionDlgHide(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    m_pBtnEmotion->SetForcePress(false, false);
    m_pBtnEmotion->UpdateObject();
    return 0;
}
LRESULT  CChatDlg2::OnInsertEmotion(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (!m_pRichEditInput)
        return 0;

    if (1==wParam)
    {
        m_pRichEditInput->InsertSkinGif(_T("chat.emotion.1"));
    }
    else if (2==wParam)
    {
        m_pRichEditInput->InsertSkinGif(_T("chat.emotion.2"));
    }
    else
    {
    //    m_pRichEditInput->InsertGif(L"C:\\aaa.gif");
    }
    return 0;
}
void  CChatDlg2::OnBtnClose()
{
    PostMessage(GetHWND(), WM_SYSCOMMAND, SC_CLOSE, 0);
}
void  CChatDlg2::OnBtnSend()
{
    if (!m_pRichEditInput || !m_pRichEditOutput)
        return;

    UI::IBuffer* pBuffer = NULL;
    m_pRichEditInput->GetEncodeTextW(&pBuffer);
    if (pBuffer)
    {
        m_pRichEditOutput->AppendEncodeTextW((const TCHAR*)pBuffer->GetBuffer(), pBuffer->GetSize());
        SAFE_RELEASE(pBuffer);
        m_pRichEditInput->SetText(_T(""));
    }

    return;
}
void  CChatDlg2::OnBtnSendMode()
{

}