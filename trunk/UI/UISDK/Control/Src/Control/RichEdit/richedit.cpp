#include "stdafx.h"
#include "richedit.h"
#include "UISDK\Kernel\Inc\Interface\icolorrender.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"
#include "UISDK\Kernel\Inc\Interface\iscrollbarmanager.h"
#include "UISDK\Control\Src\Control\ScrollBar\systemscrollbar.h"

namespace UI
{

RichEdit::RichEdit()
{
    m_pIRichEdit = NULL;
    m_pMgrScrollBar = NULL;
	m_bRedrawing = false;
}

RichEdit::~RichEdit()
{
    SAFE_DELETE(m_pMgrScrollBar);
}

HRESULT  RichEdit::FinalConstruct(IUIApplication* p)
{
	DO_PARENT_PROCESS(IRichEdit, IControl);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

    m_pMgrScrollBar = new IScrollBarManager();
	m_wrapRichEidt.BindControl(this);
    m_pMgrScrollBar->SetBindObject(m_pIRichEdit);
    m_pMgrScrollBar->SetScrollBarVisibleType(HSCROLLBAR, SCROLLBAR_VISIBLE_NONE);
    m_pMgrScrollBar->SetScrollBarVisibleType(VSCROLLBAR, SCROLLBAR_VISIBLE_AUTO);

	return S_OK;
}

void  RichEdit::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
	DO_PARENT_PROCESS(IRichEdit, IControl);
    IUIApplication*  pUIApplication = m_pIRichEdit->GetUIApplication();

    ITextRenderBase* pTextRender = NULL;
    pMapAttrib->GetAttr_TextRenderBase(NULL, XML_TEXTRENDER_TYPE, true, pUIApplication, m_pIRichEdit, &pTextRender);
	if (pTextRender)
	{
        m_pIRichEdit->SetTextRender(pTextRender);
        SAFE_RELEASE(pTextRender);
	}
	else if (NULL == m_pIRichEdit->GetTextRender())
	{
        pUIApplication->CreateTextRenderBase(TEXTRENDER_TYPE_NORMAL, m_pIRichEdit, &pTextRender);
        if (pTextRender)
        {
            pTextRender->SetAttribute(pMapAttrib, NULL, true);
            m_pIRichEdit->SetTextRender(pTextRender);
            SAFE_RELEASE(pTextRender);
        }
	}

	if (NULL == m_pIRichEdit->GetBkRender() && !m_pIRichEdit->IsTransparent())  // 如果没有指定为透明属性，则默认用窗口背景色绘制背景
	{
        IRenderBase*  pRenderBase = NULL;
        pUIApplication->CreateRenderBase(RENDER_TYPE_THEME_SYSCOLOR, m_pIRichEdit, &pRenderBase);
        if (pRenderBase)
        {
            ISysColorRender* pRender = (ISysColorRender*)pRenderBase->QueryInterface(uiiidof(ISysColorRender));
            if (pRender)
            {
                pRender->SetBkColor(COLOR_BTNFACE);
                m_pIRichEdit->SetBkgndRender(pRenderBase);
            }
            SAFE_RELEASE(pRenderBase);
        }
	}

    m_pMgrScrollBar->SetAttribute(pMapAttrib, bReload);
	m_pMgrScrollBar->SetVScrollWheel(10);

    if (!m_pMgrScrollBar->GetHScrollBar() && m_pMgrScrollBar->GetScrollBarVisibleType(HSCROLLBAR) != SCROLLBAR_VISIBLE_NONE)
    {
        ISystemHScrollBar*  p = NULL;
        ISystemHScrollBar::CreateInstance(pUIApplication, &p);
        p->SetIScrollBarMgr(m_pMgrScrollBar);
        m_pIRichEdit->AddNcChild(p);
        p->SetID(XML_HSCROLLBAR_PRIFIX);

        p->SetAttributeByPrefix(XML_HSCROLLBAR_PRIFIX, pMapAttrib, bReload, true);
        p->SetVisible(false, false, false);
    }

    if (!m_pMgrScrollBar->GetVScrollBar() && m_pMgrScrollBar->GetScrollBarVisibleType(VSCROLLBAR) != SCROLLBAR_VISIBLE_NONE)
    {
        ISystemVScrollBar*  p = NULL;
        ISystemVScrollBar::CreateInstance(pUIApplication, &p);
        p->SetIScrollBarMgr(m_pMgrScrollBar);
        m_pIRichEdit->AddNcChild(p);
        p->SetID(XML_VSCROLLBAR_PRIFIX);

        p->SetAttributeByPrefix(XML_VSCROLLBAR_PRIFIX, pMapAttrib, bReload, true);
        p->SetVisible(false, false, false);
    }
}

void  RichEdit::OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData)
{
	DO_PARENT_PROCESS(IRichEdit, IControl);

	IUIEditor* pEditor = pData->pEditor;
	const TCHAR* szPrefix = pData->szPrefix;

	IUIEditorGroupAttribute*  pRichEditGroup = pEditor->CreateGroupAttribute(pData->pGroupAttr, RichEdit::GetXmlName(), NULL);

	IUIEditorGroupAttribute* pTextRenderGroup = pEditor->CreateGroupAttribute(pRichEditGroup, XML_TEXTRENDER, NULL);
	pEditor->CreateTextAttribute(pTextRenderGroup, XML_TEXTRENDER_TYPE, szPrefix);
	if (m_pIRichEdit->GetTextRender())
	{
		EDITORGETOBJECTATTRLISTDATA  data;
		data.pEditor = pEditor;
		data.szPrefix = szPrefix;
		data.pGroupAttr = pTextRenderGroup;
		UISendMessage(m_pIRichEdit->GetTextRender(), UI_EDITOR_GETOBJECTATTRLIST, (WPARAM)&data);
	}


}

void RichEdit::ResetAttribute()
{
	DO_PARENT_PROCESS(IRichEdit, IControl);

    if (m_pMgrScrollBar)
    {
        m_pMgrScrollBar->ResetAttribute();
        m_pMgrScrollBar->SetScrollBarVisibleType(HSCROLLBAR, SCROLLBAR_VISIBLE_NONE);
        m_pMgrScrollBar->SetScrollBarVisibleType(VSCROLLBAR, SCROLLBAR_VISIBLE_AUTO);
    }
    m_pIRichEdit->ModifyStyle(OBJECT_STYLE_ENABLE_IME, 0, false);
}

void RichEdit::OnObjectLoaded()
{
	// 初始化字体(注：如果先创建textservice,再直接设置字体，
	// 调用OnTxPropertyBitsChange会崩溃
	IRenderFont* pFont = m_pIRichEdit->GetRenderFont();
	if (pFont)
	{
		LOGFONT lf;
		pFont->GetLogFont(&lf);
		m_wrapRichEidt.SetCharFormatByLogFont(&lf);
	}

	m_wrapRichEidt.Create(m_pIRichEdit->GetHWND());
}
void RichEdit::OnEraseBkgnd(IRenderTarget*  pRendrTarget)
{
	//SetMsgHandled(FALSE);
    DO_PARENT_PROCESS(IRichEdit, IControl);

    // m_caret不能放在OnEraseBkgnd中，因为RE在txDraw过程中会去隐藏、显示、设置位置等光标操作
    // 另外有可能RE还有自己的背景，会覆盖掉光标，因此将光标还是放在OnPaint中，但需要处理坐标偏移问题

    // 因为m_caret的位置是基于edit的，而不是client
    m_caret.OnControlPaint(m_pIRichEdit, pRendrTarget);

}


void RichEdit::OnPaint(IRenderTarget*  pRenderTarget, RenderContext* pContext)
{
	HDC hDC = pRenderTarget->GetHDC();
	m_wrapRichEidt.Draw(hDC, m_pIRichEdit->TestStyleEx(RICHEDIT_STYLE_TEXTSHADOW));
  
	if (pRenderTarget->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
	{
		Util::FixAlphaData data = {0};
		data.hDC = pRenderTarget->GetHDC();
		RECT  rc = pContext->m_rcCurrentClip;
		OffsetRect(&rc, pContext->m_ptBufferOffset.x, pContext->m_ptBufferOffset.y);  // 缓存还有偏移
		data.lprc = &rc;
		data.bTopDownDib = TRUE;
		data.eMode = Util::SET_ALPHA_255_IF_ALPHA_IS_0;
		Util::FixBitmapAlpha(&data);
	}
	m_wrapRichEidt.GetInvalidateRect(NULL, NULL, true);
}

// 由WindowlessRichEdit::TxInvalidateRect post过来的消息，延迟刷新
LRESULT RichEdit::OnRedrawObject(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bool bRedrawScrollbar = false;
	CRect  rcInvalid(0, 0, 0 ,0);
	if (false == m_wrapRichEidt.GetInvalidateRect(&rcInvalid, &bRedrawScrollbar, true))
		return 0;

	if (m_caret.GetCaretType()==CARET_TYPE_API)
		m_caret.HideCaret(m_pIRichEdit);  // 解决系统光标在输入文字时不显示的问题。加上Hide/Show光标即可跟随输入位置

	m_bRedrawing = true;
	{
		m_pIRichEdit->UpdateObjectEx(rcInvalid, 1, true);   
	}
	m_bRedrawing = false;

	if (m_caret.GetCaretType()==CARET_TYPE_API)
		m_caret.ShowCaret(m_pIRichEdit);

	if (bRedrawScrollbar)   // 避免每次都刷新滚动条
    {
        if (m_pMgrScrollBar->GetVScrollBar())
            UISendMessage(m_pMgrScrollBar->GetVScrollBar(), UI_WM_REDRAWOBJECT);
        if (m_pMgrScrollBar->GetHScrollBar())
            UISendMessage(m_pMgrScrollBar->GetHScrollBar(), UI_WM_REDRAWOBJECT);
    }

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
	BOOL bRet = m_wrapRichEidt.ProcessWindowMessage(NULL, uMsg, wParam, lParam, lRet);
	if (!bRet)
	{
		SetMsgHandled(FALSE);
		return 0;
	}
	return lRet;
}

void RichEdit::OnSize(UINT nType, int cx, int cy)
{
    SetMsgHandled(FALSE);

	// 更新滚动条的属性
	CRect rcClient;
	m_pIRichEdit->GetClientRect(&rcClient);

	SIZE sizeContent;
    SIZE sizePage = { rcClient.Width(), rcClient.Height() };

//	rcClient.right = rcClient.left = 0;  // 解决richedit内部，横向滚动条允许滚动到range，而不是range-page的位置
	this->m_pMgrScrollBar->GetScrollRange((int*)&sizeContent.cx, (int*)&sizeContent.cy);
	this->m_pMgrScrollBar->SetScrollPageAndRange(&sizePage, &sizeContent);
}

void RichEdit::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);  // 继续传递给windowless richedit

	if (!m_pIRichEdit->IsFocus())
	{
		IWindowBase* pWindow = m_pIRichEdit->GetWindowObject();
		if (pWindow)
		{
			pWindow->SetFocusObject(m_pIRichEdit);
		}
	}
}

// 因为richedit会自己负责刷新richedit的内容，因些
LRESULT  RichEdit::OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nSBCode = LOWORD(wParam);
	int nOldPos = 0;
	if (uMsg==WM_HSCROLL)
		nOldPos = m_pMgrScrollBar->GetHScrollPos();
	else
		nOldPos = m_pMgrScrollBar->GetVScrollPos();

	OnForwardMessage(uMsg, wParam, lParam);

	// 解决当拖拽滑块响应SB_THUMBPOSITION，richedit不会调用txSetScrollPos的问题（只会在SB_ENDSCROLL调用一次）
	if (nSBCode == SB_THUMBTRACK)
	{
		m_pIRichEdit->SetCanRedraw(false);

        UIMSG msg = *GetCurMsg();
        msg.pMsgTo = m_pMgrScrollBar;
        UISendMessage(&msg);

		m_pIRichEdit->SetCanRedraw(true);
	}

	// 刷新滚动条
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

// 不使用 mgr scroll提供的ui偏移信息，直接使用richedit内部的偏移，richedit已经实现了该功能
LRESULT RichEdit::OnGetScrollOffset(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return 0;
}


//
//	如果这里不处理的话，windowless richeidt将在收到
//  kill focus时销毁光标，但此时背景已被刷新，再调用DestroyCaret时，反而又会刷上
//  一个光标的形状，导致光标残留。
//
void RichEdit::OnShowWindow(BOOL bShow, UINT nStatus)
{
}
void RichEdit::OnVisibleChanged(BOOL bVisible, IObject* pParent)
{
    SetMsgHandled(FALSE);
	OnShowWindow(bVisible, 0);
}

HRESULT  RichEdit::OnTxNotify(DWORD iNotify, void* pv, BOOL& bHandled)
{
    UIMSG  msg;
    msg.message = UI_WM_RICHEDIT_TXNOTIFY;
    msg.wParam = iNotify;
    msg.lParam = (LPARAM)pv;
    
    long lRet = m_pIRichEdit->DoNotify(&msg);
    bHandled = msg.bHandled;
    if (msg.bHandled)
        return lRet;

    return S_OK;
}

// 代码修改自mfc源码：CFontDialog dlgfnt.cpp
void FillInLogFont(LOGFONT& m_lf, const CHARFORMAT& cf)
{
    BOOL bIsFormat2 = (cf.cbSize >= sizeof(CHARFORMAT2));

    if (cf.dwMask & CFM_SIZE)
    {
        HDC dc = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
        LONG yPerInch = ::GetDeviceCaps(dc, LOGPIXELSY);
        m_lf.lfHeight = round(-((cf.yHeight * yPerInch) / 1440.f));  // <-- 直接转成(int)会导致大小不一致，如11.6,应该转成12，而不是11
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

    m_lf.lfCharSet = DEFAULT_CHARSET;  // <-- 一些特殊字符使用ANISI_CHARSET绘制会显示不出来
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

}
