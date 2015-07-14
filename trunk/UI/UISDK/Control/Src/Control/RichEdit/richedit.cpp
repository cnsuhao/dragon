#include "stdafx.h"
#include "richedit.h"
#include "UISDK\Kernel\Inc\Interface\icolorrender.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"
#include "UISDK\Kernel\Inc\Interface\iscrollbarmanager.h"
#include "UISDK\Control\Src\Control\ScrollBar\systemscrollbar.h"
#include "UISDK\Kernel\Inc\Interface\iattribute.h"
#include "Bubble\rebubblemgr.h"

using namespace UI;


RichEdit::RichEdit()
{
    m_pIRichEdit = NULL;
    m_pMgrScrollBar = NULL;
	m_bRedrawing = false;
    m_bPostRedrawMsg = false;
    SetRectEmpty(&m_rcInvalidate);

    m_bByteLimit = false;
    m_bAutoSize = false;
    m_lSingleLineMaxWidth = NDEF;
    m_lSingleLineMinWidth = NDEF;
    m_lMultiLineMaxHeight = NDEF;
    m_lMultiLineMinHeight = NDEF;

    m_bBubbleMode = false;
}

RichEdit::~RichEdit()
{
    IScrollBarManager::DestroyIScrollBarManager(m_pMgrScrollBar);
}
void  RichEdit::SetIRichEdit(IRichEdit* p) 
{ 
    m_pIRichEdit = p; 
    p->SetWindowlessRichEdit(&m_wrapRichEidt);
    SetIMessageProxy(static_cast<IMessage*>(p));
}
IRichEdit*  RichEdit::GetIRichEdit() 
{ 
    return m_pIRichEdit; 
}

HRESULT  RichEdit::FinalConstruct(IUIApplication* p)
{
	DO_PARENT_PROCESS(IRichEdit, IControl);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

    m_pMgrScrollBar = IScrollBarManager::CreateIScrollBarManager();
	m_wrapRichEidt.BindControl(this);
    m_pMgrScrollBar->SetBindObject(m_pIRichEdit);
    m_pMgrScrollBar->SetScrollBarVisibleType(HSCROLLBAR, SCROLLBAR_VISIBLE_NONE);
    m_pMgrScrollBar->SetScrollBarVisibleType(VSCROLLBAR, SCROLLBAR_VISIBLE_AUTO);

    ObjStyle s = {0};
    s.enable_ime = 1;
    m_pIRichEdit->ModifyObjectStyle(&s, 0);

	return S_OK;
}

WindowlessRichEdit& RichEdit::GetRichEdit()
{
    return m_wrapRichEidt;
}
IScrollBarManager*  RichEdit::GetScrollMgr()
{
    return m_pMgrScrollBar;
}
CCaret*  RichEdit::GetCaret()
{
    return &m_caret;
}

void  RichEdit::OnSerialize(SERIALIZEDATA* pData)
{
	DO_PARENT_PROCESS(IRichEdit, IControl);
    IUIApplication*  pUIApplication = m_pIRichEdit->GetUIApplication();

    {
        AttributeSerializerWrap as(pData, TEXT("richedit"));
        as.AddBool(XML_RICHEDIT_MSG_BUBBLE_PREFIX XML_RICHEDIT_MSG_BUBBLE_ENABLE, this,
            memfun_cast<pfnBoolSetter>(&RichEdit::EnableBubbleMode),
            memfun_cast<pfnBoolGetter>(&RichEdit::IsBubbleMode))
            ->ReloadOnChanged();
    }

    if (IsBubbleMode())
    {
        AttributeSerializerWrap as(pData, TEXT("richedit"));
        as.AddString(
            XML_RICHEDIT_MSG_BUBBLE_PREFIX 
            XML_RICHEDIT_MSG_BUBBLE_LEFT_PREFIX 
            XML_RICHEDIT_MSG_BUBBLE_IMAGE,
            this,
            memfun_cast<pfnStringSetter>(&RichEdit::set_bubble_left_image),
            memfun_cast<pfnStringGetter>(&RichEdit::get_bubble_left_image));

        as.AddString(
            XML_RICHEDIT_MSG_BUBBLE_PREFIX 
            XML_RICHEDIT_MSG_BUBBLE_RIGHT_PREFIX 
            XML_RICHEDIT_MSG_BUBBLE_IMAGE,
            this,
            memfun_cast<pfnStringSetter>(&RichEdit::set_bubble_right_image),
            memfun_cast<pfnStringGetter>(&RichEdit::get_bubble_right_image));
    }

    m_pMgrScrollBar->Serialize(pData);
    m_pMgrScrollBar->SetVScrollWheel(10);
}


void RichEdit::OnInitialize()
{
	// ��ʼ������(ע������ȴ���textservice,��ֱ���������壬
	// ����OnTxPropertyBitsChange�����
	IRenderFont* pFont = m_pIRichEdit->GetRenderFont();
	if (pFont)
	{
		LOGFONT lf;
		pFont->GetLogFont(&lf);
		m_wrapRichEidt.SetCharFormatByLogFont(&lf, 0);
	}

	m_wrapRichEidt.Create(m_pIRichEdit->GetHWND());

    m_vscrollbarCreator.Initialize(m_pIRichEdit, m_pMgrScrollBar);
}

void  RichEdit::OnCreateByEditor(CREATEBYEDITORDATA* pData)
{
    DO_PARENT_PROCESS_MAPID(IRichEdit, IControl, UIALT_CALLLESS);
    m_vscrollbarCreator.CreateByEditor(pData, m_pIRichEdit);
}


void RichEdit::OnEraseBkgnd(IRenderTarget*  pRenderTarget)
{
	//SetMsgHandled(FALSE);
    DO_PARENT_PROCESS(IRichEdit, IControl);
}


void RichEdit::OnPaint(IRenderTarget*  pRenderTarget, RenderContext* pContext)
{
	HDC hDC = pRenderTarget->GetHDC();
	m_wrapRichEidt.Draw(hDC, m_pIRichEdit->TestStyleEx(RICHEDIT_STYLE_TEXTSHADOW));
  
    // m_caret���ܷ���OnEraseBkgnd�У���ΪRE��txDraw�����л�ȥ���ء���ʾ������λ�õȹ�����
    // �����п���RE�����Լ��ı���(������ģʽ�����Ḳ�ǵ���꣬��˽���껹�Ƿ���OnPaint�У�
    // ����Ҫ��������ƫ������(TODO:)

    m_caret.OnControlPaint(m_pIRichEdit, pRenderTarget);

	if (pRenderTarget->IsRenderAlphaChannel())
	{
		Util::FixAlphaData data = {0};
		data.hDC = pRenderTarget->GetHDC();
		RECT  rc = pContext->m_curDrawRegionInLayer;
		OffsetRect(&rc, pContext->m_ptBufferOffset.x, pContext->m_ptBufferOffset.y);  // ���滹��ƫ��
		data.lprc = &rc;
		data.bTopDownDib = TRUE;
		data.eMode = Util::SET_ALPHA_255_IF_ALPHA_IS_0;
		Util::FixBitmapAlpha(&data);
	}
}

void  RichEdit::InvalidateRect(RECT* prcWnd)
{
    if (prcWnd)
    {
        UnionRect(&m_rcInvalidate, &m_rcInvalidate, prcWnd);
    }
    else
    {
        // ע�����ﲻ��ֻȡ��ǰ�ͻ�����Ӧ�ð��������ؼ���������
        // ���б༭�����ݳ�����ʾ������ʱ���������ἷռ�ؼ�����
        // ���¿ͻ�����С����֮ǰ���Ͽͻ����ڻ��в���������Ҫˢ�µ�
        m_pIRichEdit->GetWindowRect(&m_rcInvalidate);
    }

    if (!m_bPostRedrawMsg)
    {
        m_bPostRedrawMsg = true;
        UIMSG msg;
        msg.message = UI_WM_REDRAWOBJECT;
        msg.pMsgTo = m_pIRichEdit;

        // ����RECT������post��Ϣ�в��ô��ݣ���˽����򱣴�Ϊ��Ա������ͬʱҲ�������ϲ���Ч���������
        UIPostMessage(m_pIRichEdit->GetUIApplication(), &msg);  
    }
}
void  RichEdit::InvalidateRect2(RECT* prcWndArray, int nCount)
{
    for (int i = 0; i < nCount; i++)
    {
        UnionRect(&m_rcInvalidate, &m_rcInvalidate, &prcWndArray[i]);
    }

    if (!m_bPostRedrawMsg)
    {
        m_bPostRedrawMsg = true;
        
        UIMSG msg;
        msg.message = UI_WM_REDRAWOBJECT;
        msg.pMsgTo = m_pIRichEdit;
        UIPostMessage(m_pIRichEdit->GetUIApplication(), &msg);  
    }
}

// ��WindowlessRichEdit::TxInvalidateRect post��������Ϣ���ӳ�ˢ��
LRESULT RichEdit::OnRedrawObject(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    m_bPostRedrawMsg = false;  // �յ�redraw��Ϣ�������ٴ�post redraw��Ϣ

    if (m_rcInvalidate.IsRectEmpty())
        return 0;

    if (m_caret.GetCaretType()==CARET_TYPE_API)
	    m_caret.HideCaret(m_pIRichEdit);  // ���ϵͳ�������������ʱ����ʾ�����⡣����Hide/Show��꼴�ɸ�������λ��

    // ��window����תΪobject����
    int nWidth = m_rcInvalidate.Width();
    int nHeight = m_rcInvalidate.Height();

    RECT rcDamage = {0};
    m_pIRichEdit->WindowPoint2ObjectPoint((POINT*)&m_rcInvalidate, (POINT*)&rcDamage, true);
    rcDamage.right = rcDamage.left + nWidth;
    rcDamage.bottom = rcDamage.top + nHeight;

    m_bRedrawing = true;
    {
	    m_pIRichEdit->UpdateObjectEx(&rcDamage, 1, true);   
    }
    m_bRedrawing = false;

    if (m_caret.GetCaretType()==CARET_TYPE_API)
	    m_caret.ShowCaret(m_pIRichEdit);

    m_rcInvalidate.SetRectEmpty();

	return 0;
}

// LRESULT RichEdit::OnMouseRangeMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
// {
// 	LRESULT lRet = 0;
// 	BOOL bRet = m_wrapRichEidt.ProcessWindowMessage(NULL, uMsg, wParam, lParam, lRet);
// 	if (!bRet)
// 	{
// 		SetMsgHandled(FALSE);
// 		return 0;
// 	}
// 	return lRet;
// }
void RichEdit::OnSetFocus(IObject* pOldFocusObj)
{
	LRESULT lRet = 0;
	BOOL bRet = m_wrapRichEidt.ProcessWindowMessage(NULL, WM_SETFOCUS, NULL, NULL, lRet);
	if (!bRet)
	{
		SetMsgHandled(FALSE);
		return;
	}
}
void RichEdit::OnKillFocus(IObject* pNewFocusObj)
{
	LRESULT lRet = 0;
	BOOL bRet = m_wrapRichEidt.ProcessWindowMessage(NULL, WM_KILLFOCUS, NULL, NULL, lRet);
	if (!bRet)
	{
		SetMsgHandled(FALSE);
	}
}


UINT  RichEdit::OnGetDlgCode(LPMSG lpMsg)
{
	UINT nRet = DLGC_WANTARROWS;
	nRet |= DLGC_WANTCHARS|DLGC_WANTALLKEYS;

// 	if (m_pIRichEdit->GetStyleEx() & EDIT_STYLE_WANTTAB)
// 	{
// 		if (!Util::IsKeyDown(VK_CONTROL))
// 		{
// 			nRet |= DLGC_WANTTAB;
// 		}
// 	}

	return nRet;
}


LRESULT RichEdit::OnForwardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = 0;
	BOOL bHandled = m_wrapRichEidt.ProcessWindowMessage(NULL, uMsg, wParam, lParam, lRet);
	if (!bHandled)
	{
        SetMsgHandled(FALSE);
		return 0;
	}
	return lRet;
}

void RichEdit::OnSize(UINT nType, int cx, int cy)
{
    OnForwardMessage(GetCurMsg()->message, GetCurMsg()->wParam, GetCurMsg()->lParam);

	// ���¹�����������
	CRect rcClient;
	m_pIRichEdit->GetClientRectInObject(&rcClient);

	SIZE sizeContent;
    SIZE sizePage = { rcClient.Width(), rcClient.Height() };

//	rcClient.right = rcClient.left = 0;  // ���richedit�ڲ���������������������range��������range-page��λ��
	this->m_pMgrScrollBar->GetScrollRange((int*)&sizeContent.cx, (int*)&sizeContent.cy);
	this->m_pMgrScrollBar->SetScrollPageAndRange(&sizePage, &sizeContent);
}

void RichEdit::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);  // �������ݸ�windowless richedit

	if (!m_pIRichEdit->IsFocus())
	{
		m_pIRichEdit->SetFocusInWindow();
	}
}

// ��Ϊrichedit���Լ�����ˢ��richedit������
LRESULT  RichEdit::OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nSBCode = LOWORD(wParam);
	int nOldPos = 0;
	if (uMsg==WM_HSCROLL)
		nOldPos = m_pMgrScrollBar->GetHScrollPos();
	else
		nOldPos = m_pMgrScrollBar->GetVScrollPos();

	OnForwardMessage(uMsg, wParam, lParam);

	// �������ק������ӦSB_THUMBPOSITION��richedit�������txSetScrollPos�����⣨ֻ����SB_ENDSCROLL����һ�Σ�
	if (nSBCode == SB_THUMBTRACK)
	{
		m_pIRichEdit->SetCanRedraw(false);

        UIMSG msg = *GetCurMsg();
        msg.pMsgTo = m_pMgrScrollBar;
        UISendMessage(&msg);

		m_pIRichEdit->SetCanRedraw(true);
	}

	// ˢ�¹�����
	int nPos = 0;
	if (uMsg==WM_HSCROLL)
		nPos = m_pMgrScrollBar->GetHScrollPos();
	else
		nPos = m_pMgrScrollBar->GetVScrollPos();

	if (nPos != nOldPos)
	{
		if (uMsg==WM_HSCROLL)
			UISendMessage(m_pMgrScrollBar->GetHScrollBar(), UI_WM_REDRAWOBJECT);
		else
			UISendMessage(m_pMgrScrollBar->GetVScrollBar(), UI_WM_REDRAWOBJECT);
	}

	return 0;
}

// ��ʹ�� mgr scroll�ṩ��uiƫ����Ϣ��ֱ��ʹ��richedit�ڲ���ƫ�ƣ�richedit�Ѿ�ʵ���˸ù���
LRESULT RichEdit::OnGetScrollOffset(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}


//
//	������ﲻ����Ļ���windowless richeidt�����յ�
//  kill focusʱ���ٹ�꣬����ʱ�����ѱ�ˢ�£��ٵ���DestroyCaretʱ�������ֻ�ˢ��
//  һ��������״�����¹�������
//
void RichEdit::OnShowWindow(BOOL bShow, UINT nStatus)
{
}
void RichEdit::OnVisibleChanged(BOOL bVisible, IObject* pParent)
{
    SetMsgHandled(FALSE);

    if (m_wrapRichEidt.IsFocus()) 
    {
        // ע�����ﲻ��ֱ�ӵ���m_wrapRichEidt.KillFocus������edit�����ཫ�ò���
        // killfocus֪ͨ������һЩ�߼�����ȷ��
		// ��OnObjHideInd����֤�����ﲻ�ٴ���
    //    UISendMessage(m_pIRichEdit, WM_KILLFOCUS);
    }

	OnShowWindow(bVisible, 0);
}

int  GetTextBytes(LPCTSTR szText);
int  GetCharBytes(TCHAR c);
void  RichEdit::OnEnChanged(BOOL& bHandled)
{
}

HRESULT  RichEdit::OnTxNotify(DWORD iNotify, void* pv, BOOL& bHandled)
{
    switch (iNotify)
    {
    case EN_REQUESTRESIZE:
        {
            REQRESIZE* pData = (REQRESIZE*)pv;
            OnRequestAutoSize(&pData->rc);
        }
        break;

    case EN_CHANGE:
        {
            OnEnChanged(bHandled);
            return 0;
        }
        break;

    default:
        {
            UIMSG  msg;
            msg.message = UI_WM_RICHEDIT_TXNOTIFY;
            msg.wParam = iNotify;
            msg.lParam = (LPARAM)pv;
            
            long lRet = m_pIRichEdit->DoNotify(&msg);
            bHandled = msg.bHandled;
            if (msg.bHandled)
                return lRet;
        }
        break;
    }

    return S_OK;
}

// TODO: ��Ҫ��֤�Ƿ���ȷˢ��
long  RichEdit::OnRequestAutoSize(RECT* prcRequest)
{
    int nDesiredWidth = prcRequest->right - prcRequest->left;
    int nDesiredHeight = prcRequest->bottom - prcRequest->top;

    CRegion4 rPadding;
    m_pIRichEdit->GetPaddingRegion(&rPadding);
    nDesiredWidth += rPadding.left + rPadding.right;
    nDesiredHeight += rPadding.top + rPadding.bottom;

    CRect rcWnd;
    m_pIRichEdit->GetParentRect(&rcWnd);

    bool bMultiLine = m_wrapRichEidt.IsMultiLine();
    if (bMultiLine)  // ��ȹ̶����߶�����Ӧ
    {
        if (m_lMultiLineMaxHeight > 0 && nDesiredHeight > m_lMultiLineMaxHeight)
            nDesiredHeight = m_lMultiLineMaxHeight;
        if (m_lMultiLineMinHeight > 0 && nDesiredHeight < m_lMultiLineMinHeight)
            nDesiredHeight = m_lMultiLineMinHeight;
    }
    else  // �߶ȹ̶����������Ӧ
    {
        if (m_lSingleLineMaxWidth > 0 && nDesiredWidth > m_lSingleLineMaxWidth)
            nDesiredWidth = m_lSingleLineMaxWidth;
        if (m_lSingleLineMinWidth > 0 && nDesiredWidth < m_lSingleLineMinWidth)
            nDesiredWidth = m_lSingleLineMinWidth;
    }

    // ֪ͨ�ⲿ����һ��
    {
        UIMSG  msg;
        msg.message = UI_WM_NOTIFY;
        msg.nCode = UI_EN_REQUESTAUTOSIZE;
        msg.pMsgFrom = m_pIRichEdit;
        msg.wParam = (WPARAM)prcRequest;
        msg.lParam = (LPARAM)(bMultiLine ? &nDesiredHeight : &nDesiredWidth);
        if (1 == m_pIRichEdit->DoNotify(&msg))
            return 1;
    }

    HWND hWnd = m_pIRichEdit->GetHWND();
    if (bMultiLine)
    {
        if (nDesiredHeight == rcWnd.Height())
            return 0;

        //InvalidateRect(hWnd, &rcWnd, TRUE);
        rcWnd.bottom = rcWnd.top + nDesiredHeight;
    }
    else
    {
        if (nDesiredWidth == rcWnd.Width())
            return 0;

        //InvalidateRect(hWnd, &rcWnd, TRUE);
        rcWnd.right = rcWnd.left + nDesiredWidth;
    }

    m_pIRichEdit->SetObjectPos(&rcWnd, 0/*SWP_NOREDRAW*/);
    //InvalidateRect(hWnd, &rcWnd, TRUE);

    // ֪ͨ�ⲿ����һ���Ĵ���
    {
        UIMSG  msg;
        msg.message = UI_WM_NOTIFY;
        msg.nCode = UI_EN_AUTOSIZE_DONE;
        msg.pMsgFrom = m_pIRichEdit;
        m_pIRichEdit->DoNotify(&msg);
    }
    return 1;
}

// �����޸���mfcԴ�룺CFontDialog dlgfnt.cpp
void FillInLogFont(LOGFONT& m_lf, const CHARFORMAT& cf)
{
    BOOL bIsFormat2 = (cf.cbSize >= sizeof(CHARFORMAT2));

    if (cf.dwMask & CFM_SIZE)
    {
        HDC dc = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
        LONG yPerInch = ::GetDeviceCaps(dc, LOGPIXELSY);
        m_lf.lfHeight = round(-((cf.yHeight * yPerInch) / 1440.f));  // <-- ֱ��ת��(int)�ᵼ�´�С��һ�£���11.6,Ӧ��ת��12��������11
        DeleteDC(dc);
    }
    else
    {
        m_lf.lfHeight = 0;
    }

    m_lf.lfWidth = 0;
    m_lf.lfEscapement = 0;
    m_lf.lfOrientation = 0;

    if ((cf.dwMask & (CFM_ITALIC|CFM_BOLD)) == (CFM_ITALIC|CFM_BOLD))
    {
        m_lf.lfWeight = (cf.dwEffects & CFE_BOLD) ? FW_BOLD : FW_NORMAL;
        m_lf.lfItalic = (BYTE)((cf.dwEffects & CFE_ITALIC) ? TRUE : FALSE);
    }
    else
    {
        m_lf.lfWeight = FW_DONTCARE;
        m_lf.lfItalic = FALSE;
    }

    if ((cf.dwMask & (CFM_UNDERLINE|CFM_STRIKEOUT|CFM_COLOR)) ==
        (CFM_UNDERLINE|CFM_STRIKEOUT|CFM_COLOR))
    {
        m_lf.lfUnderline = (BYTE)((cf.dwEffects & CFE_UNDERLINE) ? TRUE : FALSE);
        m_lf.lfStrikeOut = (BYTE)((cf.dwEffects & CFE_STRIKEOUT) ? TRUE : FALSE);
    }
    else
    {
        m_lf.lfUnderline = (BYTE)FALSE;
        m_lf.lfStrikeOut = (BYTE)FALSE;
    }

    m_lf.lfCharSet = DEFAULT_CHARSET;  // <-- һЩ�����ַ�ʹ��ANISI_CHARSET���ƻ���ʾ������
    if (cf.dwMask & CFM_CHARSET)
        m_lf.lfCharSet = cf.bCharSet;

    m_lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
    m_lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
    m_lf.lfQuality = DEFAULT_QUALITY;

    if (bIsFormat2)
    {
        const CHARFORMAT2* pCF2 = (const CHARFORMAT2*) &cf;
        if (pCF2->dwMask & CFM_WEIGHT)
            m_lf.lfWeight = pCF2->wWeight;

        // CHAFORMAT2 has UNICODE face name, CHARFORMAT doesn't

        if (cf.dwMask & CFM_FACE)
        {
            m_lf.lfPitchAndFamily = pCF2->bPitchAndFamily;
            Checked::tcscpy_s(m_lf.lfFaceName, _countof(m_lf.lfFaceName), (LPTSTR)pCF2->szFaceName);
        }
    }
    else
    {
        m_lf.lfPitchAndFamily = cf.bPitchAndFamily;
        String strFaceName(cf.szFaceName);
        Checked::tcsncpy_s(m_lf.lfFaceName, _countof(m_lf.lfFaceName), strFaceName.c_str(), _TRUNCATE);
    }

    if (!(cf.dwMask & CFM_FACE))
    {
        m_lf.lfPitchAndFamily = DEFAULT_PITCH|FF_DONTCARE;
        m_lf.lfFaceName[0] = (TCHAR)0;
    }
}

void  RichEdit::SetReadOnly(bool fReadOnly)
{
    if (fReadOnly == m_wrapRichEidt.IsReadOnly())
        return;

    m_wrapRichEidt.SetReadOnly(fReadOnly);
    UISendMessage(m_pIRichEdit, UI_WM_STATECHANGED2, OSB_READONLY);
}

void  RichEdit::EnableBubbleMode(bool b)
{
    m_bBubbleMode = b;

    if (m_vscrollbarCreator.m_pVScrollBar)
    {
        m_vscrollbarCreator.m_pVScrollBar->SetKeepHoldNonClientRegion(m_bBubbleMode);
    }
    m_wrapRichEidt.EnableBubbleMode(b);
}
bool  RichEdit::IsBubbleMode()
{
    return m_bBubbleMode;
}

void  RichEdit::set_bubble_left_image(LPCTSTR szText)
{
    if (!m_wrapRichEidt.m_pBubbleMgr)
        return;

    ISkinRes*  pSkinRes = m_pIRichEdit->GetUIApplication()->GetActiveSkinRes();
    IImageRes* pImageRes = pSkinRes->GetImageRes();
    IImageResItem* pItem = pImageRes->GetImageResItem(szText);
    
    m_wrapRichEidt.m_pBubbleMgr->SetLeftImageResItem(pItem);

}
void  RichEdit::set_bubble_right_image(LPCTSTR szText)
{
    if (!m_wrapRichEidt.m_pBubbleMgr)
        return;

    ISkinRes*  pSkinRes = m_pIRichEdit->GetUIApplication()->GetActiveSkinRes();
    IImageRes* pImageRes = pSkinRes->GetImageRes();
    IImageResItem* pItem = pImageRes->GetImageResItem(szText);

    m_wrapRichEidt.m_pBubbleMgr->SetRightImageResItem(pItem);

}

LPCTSTR  RichEdit::get_bubble_left_image()
{
    if (!m_wrapRichEidt.m_pBubbleMgr)
        return NULL;

    IImageResItem* pItem = m_wrapRichEidt.m_pBubbleMgr->GetLeftImageResItem();
    if (!pItem)
        return NULL;

    return pItem->GetId();
}

LPCTSTR  RichEdit::get_bubble_right_image()
{
    if (!m_wrapRichEidt.m_pBubbleMgr)
        return NULL;

    IImageResItem* pItem = m_wrapRichEidt.m_pBubbleMgr->GetRightImageResItem();
    if (!pItem)
        return NULL;

    return pItem->GetId();
}


// ��Ϊһ�������п��ܹ����ж��re�����ڲ����ʺ���re�Լ���ע�ᴰ��IDropTarget
// ͳһ��uisdk�е�window dragdrop mgr��Ȼ����Ϣת����re
void  RichEdit::RegisterDropEvent()
{
    ObjStyle s = {0};
    s.receive_dragdrop_event = 1;
    m_pIRichEdit->ModifyObjectStyle(&s, NULL);

    IWindowBase* pWindow = m_pIRichEdit->GetWindowObject();
    if (pWindow)
        pWindow->SetDroppable(true);
}
void  RichEdit::RevokeDropEvent()
{
    ObjStyle s = {0};
    s.receive_dragdrop_event = 1;
    m_pIRichEdit->ModifyObjectStyle(NULL, &s);
}