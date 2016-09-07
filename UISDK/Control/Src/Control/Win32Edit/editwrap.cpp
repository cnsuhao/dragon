#include "stdafx.h"
#ifdef COMPILE_Win32Edit
#include "editwrap.h"
#include "Win32Edit.h"
#include "UISDK\Control\Inc\Interface\imenu.h"

using namespace UI;

EditWrap::EditWrap(Win32Edit* pControl)
{
    m_hWnd = NULL;
    m_hWndParent = NULL;
    m_pWin32Edit = pControl;
    m_nFontHeight = 20;
    m_colorBkg = RGB(255,255,255);
    SetRectEmpty(&m_rcScreen);
    m_bDestroying = false;
    m_lLimitText = 0;
    m_bByteLimit = false;
}

EditWrap::~EditWrap()
{
    UIASSERT(NULL == m_hWnd);
}



LRESULT CALLBACK  EditWrap::ThunkWndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    EditWrap* pThis = (EditWrap*)hwnd;
    return pThis->WndProc( uMsg, wParam, lParam );
}
LRESULT	EditWrap::WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch (uMsg)
    {

    case WM_KILLFOCUS:
        {
            // 有可能是AER_REJEST_AND_CONTINUE_EDIT中用SetFocus调用导致自己又收到一次killfocus消息
            // 但wParam还是指向自己
            if (m_hWnd != (HWND)wParam)
                this->Destroy(EDF_ACCEPT);
            return 0;
        }
        break;

    case WM_DESTROY:
        {
            m_hWnd = NULL;
            if (m_pWin32Edit)
            {
                m_pWin32Edit->OnEditWindowDestroy();
                return 0; 
            }
        }
        break;

    case WM_ERASEBKGND:
        {
            return 1;
            HBRUSH hBrush = CreateSolidBrush(m_colorBkg);
            RECT rc;
            ::GetClientRect(m_hWnd, &rc);
            ::FillRect((HDC)wParam, &rc, hBrush);
            SAFE_DELETE_GDIOBJECT(hBrush);
            return 1;
        }
        break;  

    case WM_NCPAINT:
        {
            HDC hdc = GetWindowDC(m_hWnd); // GetDCEx(m_hWnd, (HRGN)wParam, DCX_WINDOW|DCX_INTERSECTRGN);

            RECT rc;
            ::GetWindowRect(m_hWnd, &rc);

            RECT rcClient;
            ::GetClientRect(m_hWnd, &rcClient);
            ::MapWindowPoints(m_hWnd, NULL, (LPPOINT)&rcClient, 2);
            ::OffsetRect(&rcClient, -rc.left, -rc.top);
            ::OffsetRect(&rc, -rc.left, -rc.top);

            HBRUSH hBrush = CreateSolidBrush(m_colorBkg);

            RECT rcLeft = rc;
            rcLeft.right = rcClient.left;
            if (!IsRectEmpty(&rcLeft))
                ::FillRect(hdc, &rcLeft, hBrush);

            RECT rcRight = rc;
            rcRight.left = rcClient.right;
            if (!IsRectEmpty(&rcRight))
                ::FillRect(hdc, &rcRight, hBrush);

            RECT rcTop = rc;
            rcTop.bottom = rcClient.top;
            if (!IsRectEmpty(&rcTop))
                ::FillRect(hdc, &rcTop, hBrush);

            RECT rcBottom = rc;
            rcBottom.top = rcClient.bottom;
            if (!IsRectEmpty(&rcBottom))
                ::FillRect(hdc, &rcBottom, hBrush);

            SAFE_DELETE_GDIOBJECT(hBrush);
            ReleaseDC(m_hWnd, hdc);
            return 0;
        }
        break;

    case WM_NCCALCSIZE:
        {
            // 实现文字纵向居中显示
            RECT* prc = (RECT*)lParam;

            int nTop = prc->top + 
                (((prc->bottom-prc->top) - m_nFontHeight)>>1);
            int nBottom = nTop + m_nFontHeight;

            prc->top = nTop;
            prc->bottom = nBottom;
            return 0;
        }
        break;

    case WM_PAINT:
        {
            LRESULT lr = m_oldWndProc(m_hWnd, uMsg, wParam, lParam);
            return lr;
        }
        break;

    case WM_KEYDOWN:
        {
            if (wParam == VK_ESCAPE)
            {
                this->Destroy(EDF_CANCEL);
                return 0;
            }
            else if (wParam == VK_RETURN)
            {
                this->Destroy(EDF_ACCEPT);
                return 0;
            }
            else if (wParam == TEXT('A'))
            {
				if (Util::IsKeyDown(VK_CONTROL))
                {
                    SendMessage(m_hWnd, EM_SETSEL, 0, -1);
                    return 0;
                }
            }
        }
        break;

    case WM_CONTEXTMENU:
        {
            OnContextMenu();
            return 0;
        }
        break;

    case WM_CHAR:
        {
            bool bHandled = false;
            OnChar((UINT)wParam, bHandled);

            if (bHandled)
                return 0;
        }
        break;

	case WM_PASTE:
		{
			bool bHandled = false;
			OnPaste(bHandled);

			if (bHandled)
				return 0;
		}
		break;
    }
    return m_oldWndProc(m_hWnd, uMsg, wParam, lParam);
}

bool EditWrap::Create( RECT* prc, HWND hWndParent )
{
    if (m_hWnd || !prc)
        return false;

    m_hWnd = CreateWindowEx(WS_EX_NOACTIVATE|WS_EX_TOOLWINDOW,
        TEXT("Edit"), _T(""), WS_POPUP|ES_AUTOHSCROLL|WS_VISIBLE, 
        0, 0, 0, 0,
        hWndParent, NULL, NULL, this);

    if (!m_hWnd)
        return false;

    m_hWndParent = hWndParent;
    m_thunk.Init(EditWrap::ThunkWndProc, this);
    WNDPROC pProc = m_thunk.GetWNDPROC();
    m_oldWndProc = (WNDPROC)::SetWindowLong(m_hWnd, GWLP_WNDPROC, 
        (LONG)(LONG_PTR)pProc);

    CopyRect(&m_rcScreen, prc);
    return true;
}

void  EditWrap::Show()
{
    SetWindowPos(m_hWnd, 0,
        -32000, -32000,
        /*m_rcScreen.left, m_rcScreen.top,*/
        m_rcScreen.right - m_rcScreen.left,
        m_rcScreen.bottom - m_rcScreen.top,
        SWP_NOZORDER|SWP_NOACTIVATE);
    UpdateWindow(m_hWnd);

    SetWindowPos(m_hWnd, 0,
        m_rcScreen.left, m_rcScreen.top,
        0, 0,
        SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSIZE);
    UpdateWindow(m_hWnd);
}
void  EditWrap::Hide()
{
    ShowWindow(m_hWnd, SW_HIDE);  // 如果正在显示，也会导致失焦
}
void  EditWrap::Destroy(EditDestroyFlag eFlag)
{
    // 避免一些操作触发KILLFOCUS，导致又进入一次Destroy函数
    if (m_bDestroying)
        return;

    m_bDestroying = true;
    if (m_hWnd)
    {
        ACCEPT_EDIT_RESULT ret = (ACCEPT_EDIT_RESULT)m_pWin32Edit->OnEditWindowPreDestroy(m_hWnd, eFlag);
        if (ret == AER_REJECT_AND_CONTINUE_EDIT)
        {
            MSG msg = {0};
            BOOL bRet = PeekMessage(&msg, m_hWnd, WM_KILLFOCUS, WM_KILLFOCUS, PM_REMOVE);

            m_bDestroying = false;
            SetFocus(m_hWnd);
            return;
        }
        Hide();

        HWND hWndParent = m_hWndParent;
        BOOL b = GetActiveWindow() == m_hWnd;
        
        DestroyWindow(m_hWnd); 
        
        if (hWndParent && b)
            SetActiveWindow(hWndParent);

        delete this;
        return;
    }
}

void  EditWrap::SetFont(HFONT hFont)
{
    if (!m_hWnd)
        return;

    SendMessage(m_hWnd, WM_SETFONT, (WPARAM)hFont, FALSE);

    SIZE size = {0};

    HDC hDC = ::GetDC(m_hWnd);
    HFONT hOldFont = (HFONT)::SelectObject(hDC, hFont);
    ::GetTextExtentPoint32(hDC, _T("A"), 1, &size);
    SelectObject(hDC, hOldFont);
    ::ReleaseDC(m_hWnd, hDC);

    m_nFontHeight = size.cy;
}

void  EditWrap::SetText(LPCTSTR szText, bool bSelAll)
{
    if (szText)
    {
        SendMessage(m_hWnd, WM_SETTEXT, 0, (LPARAM)szText);
        if (bSelAll)
        {
            SendMessage(m_hWnd, EM_SETSEL, 0, -1);
        }
        else
        {
            int nLength = _tcslen(szText);
            SendMessage(m_hWnd, EM_SETSEL, nLength, nLength);
        }
    }
}

void  EditWrap::OnContextMenu()
{
    if (!m_pWin32Edit)
        return;

    IUIApplication* pUIApp = m_pWin32Edit->GetIWin32Edit()->GetUIApplication();

	UI::LoadMenuData data = {pUIApp, 0};
    UI::IMenu* pMenu = UILoadMenu(&data);
    if (!pMenu)
        return;

#define MENU_ID_CUT     1
#define MENU_ID_COPY    2
#define MENU_ID_PASTE   3
#define MENU_ID_SELALL  4

    POINT pt;
    GetCursorPos(&pt);

    BOOL bReadOnly =  ::GetWindowLong(m_hWnd, GWL_STYLE) & ES_READONLY;
    int nSelStart = 0, nSelEnd = 0;
    SendMessage(m_hWnd, EM_GETSEL, (WPARAM)&nSelStart, (LPARAM)&nSelEnd);
    BOOL bHasSelection = nSelEnd != nSelStart;

    IMenuStringItem*  pCutItem = NULL;
    IMenuStringItem*  pCopyItem = NULL;
    IMenuStringItem*  pPasteItem = NULL;
    IMenuStringItem*  pSelAllItem = NULL;

    if (bReadOnly)
    {
        pCopyItem = pMenu->AppendString(TEXT("复制"), MENU_ID_COPY);
        pSelAllItem = pMenu->AppendString(TEXT("全选"), MENU_ID_SELALL);
    }
    else
    {
        pCutItem = pMenu->AppendString(TEXT("剪切"), MENU_ID_CUT);
        pCopyItem = pMenu->AppendString(TEXT("复制"), MENU_ID_COPY);
        pPasteItem = pMenu->AppendString(TEXT("粘贴"), MENU_ID_PASTE);
        pMenu->AppendSeparator(-1);
        pSelAllItem = pMenu->AppendString(TEXT("全选"), MENU_ID_SELALL);
    }
    
    // 条件判断

    // 剪切
    if (pCutItem)
    {
        if (bReadOnly ||!bHasSelection)
            pCutItem->SetDisable(true, false);
    }

    // 复制
    if (pCopyItem)
    {
        if (!bHasSelection)
            pCopyItem->SetDisable(true, false);
    }

    // 粘贴
    if (pPasteItem)
    {
        if (bReadOnly || (!IsClipboardFormatAvailable(CF_TEXT) && !IsClipboardFormatAvailable(CF_UNICODETEXT)))
            pPasteItem->SetDisable(true, false);
    }

    // 全选
    if (pSelAllItem)
    {
        if (::GetWindowTextLength(m_hWnd))
            pSelAllItem->SetDisable(true, false);
    }

    long lRet = pMenu->TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, NULL);   
    switch (lRet)
    {
    case MENU_ID_CUT:
        SendMessage(m_hWnd, WM_CUT, 0, 0);
        break;
    case MENU_ID_COPY:
        SendMessage(m_hWnd, WM_COPY, 0, 0);
        break;
    case MENU_ID_PASTE: 
        SendMessage(m_hWnd, WM_PASTE, 0, 0);
        break;
    case MENU_ID_SELALL:
        SendMessage(m_hWnd, EM_SETSEL, 0, -1);
        break;
    }
	UIDestroyMenu(pMenu);
}

void  EditWrap::SetLimitText(int lLimitText, bool bByteLimit)
{
    m_lLimitText = lLimitText;
    m_bByteLimit = bByteLimit;
    if (m_lLimitText > 0)
    {
        if (m_bByteLimit)
            SendMessage(m_hWnd, EM_SETLIMITTEXT, (WPARAM)(m_lLimitText>>1), 0);
        else
            SendMessage(m_hWnd, EM_SETLIMITTEXT, (WPARAM)m_lLimitText, 0);
    }
}

int  GetTextBytes(LPCTSTR  szText)
{
	if (!szText || 0 == szText[0])
		return 0;

    return strlen(CT2A(szText));
}
int  GetCharBytes(TCHAR c)
{
    TCHAR p[2] = {c, 0};
    return strlen(CT2A(p));
}
bool IsFunctionKey(UINT key)
{
    switch(key)
    {
    case VK_BACK:
    case VK_TAB:
    case VK_CLEAR:
    case VK_RETURN:
    case VK_ESCAPE:
    case VK_CAPITAL:
    case VK_PAUSE:
    case VK_NUMLOCK:
        return true;
    };
    return false;
}

void  GetClipboardString(String &str, HWND hWnd)
{
	if (IsClipboardFormatAvailable(CF_UNICODETEXT))
    {
	    if (!OpenClipboard(hWnd))
		    return;

	    HGLOBAL hBuffer = GetClipboardData( CF_UNICODETEXT );

	    wchar_t* szText = (wchar_t*)GlobalLock( hBuffer );
	    if (szText)
		    str = CW2T(szText);

	    GlobalUnlock( hBuffer );
	    CloseClipboard();
    }
    else if (IsClipboardFormatAvailable(CF_TEXT))
    {
        if (!OpenClipboard(hWnd))
            return;

        HGLOBAL hBuffer = GetClipboardData( CF_TEXT );

        char* szText = (char*)GlobalLock( hBuffer );
        if (szText)
            str = CA2T(szText);

        GlobalUnlock( hBuffer );
        CloseClipboard();
    }
}

void  EditWrap::GetEditTextAndSelText(String& str, String& strSel)
{
    int nTextLength = GetWindowTextLength(m_hWnd) + 1;

    TCHAR*  pszText = new TCHAR[nTextLength];
    memset(pszText, 0, sizeof(TCHAR)*nTextLength);
    GetWindowText(m_hWnd, pszText, nTextLength);

    str = pszText;
    SAFE_ARRAY_DELETE(pszText);

	if (!str.empty())
	{
		int  nSelStart = 0;
		int  nSelEnd = 0;
		SendMessage(m_hWnd, EM_GETSEL, (WPARAM)&nSelStart, (LPARAM)&nSelEnd);

		if (nSelEnd != nSelStart)
		{
			strSel = str.substr(nSelStart, nSelEnd-nSelStart);
		}
	}
}

void  EditWrap::OnChar(UINT nChar, bool& bHandled)
{
    bHandled = false;
    if (!m_bByteLimit)
        return;
    if (IsFunctionKey(nChar))
        return;

    String strText;
	String strSelection;
    GetEditTextAndSelText(strText, strSelection);

    int nBytesNow = GetTextBytes(strText.c_str());
	int nBytesSel = GetTextBytes(strSelection.c_str());
    int nBytesInput = GetCharBytes(nChar);

    if ((nBytesInput + nBytesNow - nBytesSel) > (m_lLimitText) )
    {
        // 不允许该文字输入
		bHandled = true;
        //SendMessage(m_hWnd, EM_SETLIMITTEXT, (WPARAM)nTextLengthNow, 0);
    }
    else
    {
        // 允许文字输入
		int nTextLengthNow = (int)(strText.length() - strSelection.length());
        SendMessage(m_hWnd, EM_SETLIMITTEXT, (WPARAM)nTextLengthNow+1, 0);
    }
}

void  EditWrap::OnPaste(bool& bHandled)
{
	bHandled = false;
	if (!m_bByteLimit)
		return;

	String strPaste;
	GetClipboardString(strPaste, m_hWnd);

	String strText;
	String strSelection;
	GetEditTextAndSelText(strText, strSelection);

	int nBytesNow = GetTextBytes(strText.c_str());
	int nBytesSel = GetTextBytes(strSelection.c_str());
	int nBytesInput = GetTextBytes(strPaste.c_str());

	// 最大限度了，不能再输入
	int nCanInputBytes = m_lLimitText - (nBytesNow - nBytesSel);
	if (nCanInputBytes <= 0)
	{
		bHandled = true;
		return;
	}

	// 可以完全输入
	if (nBytesInput <= nCanInputBytes)
	{
		bHandled = false;

		int nNewLimit = (int)(strText.length() - strSelection.length() + strPaste.length());
		SendMessage(m_hWnd, EM_SETLIMITTEXT, nNewLimit, 0);
		return;
	}

	// 测试能输入几个
	string  strAnsii(CT2A(strPaste.c_str()));

	int nTestBytes = 0;
	int nTestLength = 0;
	LPCSTR p  = strAnsii.c_str();
	LPCSTR p2 = p;
	while (1)
	{
		p2 = ::CharNextA(p);
		if (p2 == p)
			break;

		nTestBytes += p2-p;
		if (nTestBytes > nCanInputBytes)
			break;

		nTestLength+= 1;
		p = p2;
	}

	if (0 == nTestLength)
	{
		// 有可能只剩下1byte了，但第一个字是汉字无法输入
		bHandled = true;
		return;
	}
	int nTextLengthNow = (int)(strText.length() - strSelection.length());
	SendMessage(m_hWnd, EM_SETLIMITTEXT, (WPARAM)nTextLengthNow+nTestLength, 0);
}

#endif