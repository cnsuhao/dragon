#include "stdafx.h"
#include "layeredwrap.h"
#include "UISDK/Kernel/Src/UIObject/Window/customwindow.h"
#include "UISDK/Kernel/Src/RenderLayer/renderchain.h"

#define WINDOWS_MINIMIZED_POINT  -32000  // ������С����window���ݸ����ǵ�λ��
namespace UI
{


LayeredWindowWrap::LayeredWindowWrap()
{
    m_nHitTestFlag = 0;
    m_ptStartSizeMove.x = 0;
    m_ptStartSizeMove.y = 0;
    m_ptWindowOld.x = 0;
    m_ptWindowOld.y = 0;
    m_sizeWindowOld.cx = 0;
    m_sizeWindowOld.cy = 0;
    m_ptWindow.x = NDEF;
    m_ptWindow.y = NDEF;
    m_sizeWindow.cx = NDEF;
    m_sizeWindow.cy = NDEF;
    m_nAlphaValue = 255;
    m_pILayeredWindowWrap = NULL;
}
LayeredWindowWrap::~LayeredWindowWrap()
{
    SAFE_DELETE(m_pILayeredWindowWrap);
}

void  LayeredWindowWrap::InitLayeredWindow()
{
    CRect rc;
    ::GetWindowRect(m_pWindow->m_hWnd, &rc );

    // ������Ϊ��SetLayerWindowd(true)֮ǰ����˴�������SIZE��WindowPosChanged��Ϣ��
    // ��������һ��
    if (NDEF == m_sizeWindow.cx && NDEF == m_sizeWindow.cy)
    {
        m_sizeWindow.cx = rc.Width();
        m_sizeWindow.cy = rc.Height();
    }
    if (NDEF == m_ptWindow.x && NDEF == m_ptWindow.y)
    {
        m_ptWindow.x = rc.left;
        m_ptWindow.y = rc.top;
    }
}


// ģ����ק�����������
void  LayeredWindowWrap::OnLButtonDown(UINT nHitTest)
{
    OnEnterSizeMove(nHitTest);
}
void  LayeredWindowWrap::OnLButtonUp(UINT nFlags, POINT point)
{
    OnExitSizeMove();
}
void  LayeredWindowWrap::OnMouseMove(UINT nFlags, POINT point)
{
    if (0 == m_nHitTestFlag)
        return;

    POINT ptCursor;
    GetCursorPos(&ptCursor);

    int nxMoveDiff = 0;
    int nyMoveDiff = 0;

    int oldCX = m_sizeWindow.cx;
    int oldCY = m_sizeWindow.cy;

    // ���㴰�ڵ������� (ע����������/������ʱ���������������/�ߣ����ڼ�������ʱ��Ҫ��������/�������㣬���������λ��)
    switch(m_nHitTestFlag)
    {
    case HTLEFT:
        nxMoveDiff = m_ptStartSizeMove.x - ptCursor.x;
        m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;

        if (m_pWindow->m_nMaxWidth != NDEF && m_sizeWindow.cx > m_pWindow->m_nMaxWidth)
            m_sizeWindow.cx = m_pWindow->m_nMaxWidth;
        if (m_pWindow->m_nMinWidth != NDEF && m_sizeWindow.cx < m_pWindow->m_nMinWidth)
            m_sizeWindow.cx = m_pWindow->m_nMinWidth;

        m_ptWindow.x = m_ptWindowOld.x + m_sizeWindowOld.cx - m_sizeWindow.cx;

        break;

    case HTRIGHT:
        nxMoveDiff = ptCursor.x - m_ptStartSizeMove.x;
        m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;
        break;

    case HTTOP:
        nyMoveDiff = m_ptStartSizeMove.y - ptCursor.y;
        m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;

        if (m_pWindow->m_nMaxHeight != NDEF && m_sizeWindow.cy > m_pWindow->m_nMaxHeight)
            m_sizeWindow.cy = m_pWindow->m_nMaxHeight;
        if (m_pWindow->m_nMinHeight != NDEF && m_sizeWindow.cy < m_pWindow->m_nMinHeight)
            m_sizeWindow.cy = m_pWindow->m_nMinHeight;

        m_ptWindow.y = m_ptWindowOld.y+m_sizeWindowOld.cy - m_sizeWindow.cy;

        break;

    case HTBOTTOM:
        nyMoveDiff = ptCursor.y - m_ptStartSizeMove.y;
        m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;
        break;

    case HTTOPLEFT:
        nxMoveDiff = m_ptStartSizeMove.x - ptCursor.x;
        m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;

        nyMoveDiff = m_ptStartSizeMove.y - ptCursor.y;
        m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;

        if (m_pWindow->m_nMaxWidth != NDEF && m_sizeWindow.cx > m_pWindow->m_nMaxWidth)
            m_sizeWindow.cx = m_pWindow->m_nMaxWidth;
        if (m_pWindow->m_nMaxHeight != NDEF && m_sizeWindow.cy > m_pWindow->m_nMaxHeight)
            m_sizeWindow.cy = m_pWindow->m_nMaxHeight;
        if (m_pWindow->m_nMinWidth != NDEF && m_sizeWindow.cx < m_pWindow->m_nMinWidth)
            m_sizeWindow.cx = m_pWindow->m_nMinWidth;
        if (m_pWindow->m_nMinHeight != NDEF && m_sizeWindow.cy < m_pWindow->m_nMinHeight)
            m_sizeWindow.cy = m_pWindow->m_nMinHeight;

        m_ptWindow.y = m_ptWindowOld.y+m_sizeWindowOld.cy - m_sizeWindow.cy;
        m_ptWindow.x = m_ptWindowOld.x + m_sizeWindowOld.cx - m_sizeWindow.cx;

        break;

    case HTTOPRIGHT:
        nxMoveDiff = ptCursor.x - m_ptStartSizeMove.x;
        m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;

        nyMoveDiff = m_ptStartSizeMove.y - ptCursor.y;
        m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;

        if (m_pWindow->m_nMaxHeight != NDEF && m_sizeWindow.cy > m_pWindow->m_nMaxHeight)
            m_sizeWindow.cy = m_pWindow->m_nMaxHeight;
        if (m_pWindow->m_nMinHeight != NDEF && m_sizeWindow.cy < m_pWindow->m_nMinHeight)
            m_sizeWindow.cy = m_pWindow->m_nMinHeight;

        m_ptWindow.y = m_ptWindowOld.y+m_sizeWindowOld.cy - m_sizeWindow.cy;
        break;

    case HTBOTTOMLEFT:
        nxMoveDiff = m_ptStartSizeMove.x - ptCursor.x;
        m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;

        if (m_pWindow->m_nMaxWidth != NDEF && m_sizeWindow.cx > m_pWindow->m_nMaxWidth)
            m_sizeWindow.cx = m_pWindow->m_nMaxWidth;
        if (m_pWindow->m_nMinWidth != NDEF && m_sizeWindow.cx < m_pWindow->m_nMinWidth)
            m_sizeWindow.cx = m_pWindow->m_nMinWidth;

        m_ptWindow.x = m_ptWindowOld.x + m_sizeWindowOld.cx - m_sizeWindow.cx;

        nyMoveDiff = ptCursor.y - m_ptStartSizeMove.y;
        m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;
        break;

    case HTBOTTOMRIGHT:
        nxMoveDiff = ptCursor.x - m_ptStartSizeMove.x;
        m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;

        nyMoveDiff = ptCursor.y - m_ptStartSizeMove.y;
        m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;
        break;
    }

    // ���ƴ��ڴ�С
    if (m_pWindow->m_nMaxWidth != NDEF && m_sizeWindow.cx > m_pWindow->m_nMaxWidth)
        m_sizeWindow.cx = m_pWindow->m_nMaxWidth;
    if (m_pWindow->m_nMaxHeight != NDEF && m_sizeWindow.cy > m_pWindow->m_nMaxHeight)
        m_sizeWindow.cy = m_pWindow->m_nMaxHeight;
    if (m_pWindow->m_nMinWidth != NDEF && m_sizeWindow.cx < m_pWindow->m_nMinWidth)
        m_sizeWindow.cx = m_pWindow->m_nMinWidth;
    if (m_pWindow->m_nMinHeight != NDEF && m_sizeWindow.cy < m_pWindow->m_nMinHeight)
        m_sizeWindow.cy = m_pWindow->m_nMinHeight;

    if (oldCX == m_sizeWindow.cx && oldCY == m_sizeWindow.cy)
    {
        return;
    }


    m_pWindow->SetCanRedraw(false);  // ���汻��գ���ֹ���ڼ���object redraw,�ύ������ʱ�����ݲ�������
    //m_pWindow->CreateDoubleBuffer(m_sizeWindow.cx, m_sizeWindow.cy);
    m_pWindow->m_pRenderChain->OnWindowResize(0, m_sizeWindow.cx, m_sizeWindow.cy);

    // ע�⣺m_rcParent�ĸ���ǧ����ʹ��GetWindowRect����Ϊ���ڵĴ�С���ھ�û�б�
    //       ��������Ҳ��û�в���SendMessage(WM_SIZE)�ķ���
    SetRect(&m_pWindow->m_rcParent, 0,0, m_sizeWindow.cx, m_sizeWindow.cy);

    m_pWindow->SetConfigWidth(m_sizeWindow.cx);
    m_pWindow->SetConfigHeight(m_sizeWindow.cy);

    m_pWindow->UpdateLayout(false);

    m_pWindow->SetCanRedraw(true);
    m_pWindow->UpdateObject();

    // ģ�ⴰ�ڴ�С�ı���Ϣ
    // PS: 20130108 17:07 ����Ϣ���͵�ʱ���Ƶ�OnSize֮��
    //     ��Ҫ����Ϊ��OnSize�У��ᴥ��richedit��setcaretpos֪ͨ��Ȼ���ʱ���ڵĴ�С��û�������ı䣬
    //     �����CCaretWindow::SetCaretPos�е���MapWindowPoints��ȡ���Ĺ����Ļλ�û��Ǿɵģ����¹��
    //     ���洰���ƶ�ʧ�ܡ���˽�CCaretWindow::OnSyncWindowPosChanging�Ӻ�������������⡣
    {
        MSG  msg;
        msg.hwnd = m_pWindow->m_hWnd;
        msg.message = WM_WINDOWPOSCHANGING;
        msg.wParam = 0;

        WINDOWPOS  wndpos;
        memset(&wndpos, 0, sizeof(&wndpos));
        wndpos.hwnd = msg.hwnd;
        wndpos.flags = SWP_LAYEREDWINDOW_SIZEMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSENDCHANGING;
        wndpos.x = m_ptWindow.x;
        wndpos.y = m_ptWindow.y;
        wndpos.cx = m_sizeWindow.cx;
        wndpos.cy = m_sizeWindow.cy;
        msg.lParam = (LPARAM)&wndpos;
        ::SendMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam);
    }
}
void  LayeredWindowWrap::OnEnterSizeMove(UINT nHitTest)
{
    SendMessage(m_pWindow->m_hWnd, WM_ENTERSIZEMOVE, 0, 0);
    SetCapture(m_pWindow->m_hWnd);
    m_nHitTestFlag = nHitTest;

    POINT ptWindow;
    GetCursorPos(&ptWindow);

    m_ptStartSizeMove.x = ptWindow.x;
    m_ptStartSizeMove.y = ptWindow.y;

    m_ptWindowOld.x = m_ptWindow.x;
    m_ptWindowOld.y = m_ptWindow.y;
    m_sizeWindowOld.cx = m_sizeWindow.cx;
    m_sizeWindowOld.cy = m_sizeWindow.cy;
}

void  LayeredWindowWrap::OnExitSizeMove()
{
    if (0 == m_nHitTestFlag)
        return;

    if (m_pWindow->m_hWnd == GetCapture())
        ReleaseCapture();

    m_nHitTestFlag = 0;
    m_ptStartSizeMove.x = 0;
    m_ptStartSizeMove.y = 0;

    m_ptWindowOld.x = 0;
    m_ptWindowOld.y = 0;
    m_sizeWindowOld.cx = 0;
    m_sizeWindowOld.cy = 0;
    SendMessage(m_pWindow->m_hWnd, WM_EXITSIZEMOVE, 0, 0);
}

// ��������С���˵�ʱ������ٴ���ԭpoint/size, Commit���ᵼ�´����ָֻ�����С��֮ǰ��λ��
// �������һ����־�����������С���ˣ����޸Ĵ���λ�ã�����ԭ���ڴ�С�����ύ
bool  LayeredWindowWrap::IsMinimized()
{
    if (m_ptWindow.x == WINDOWS_MINIMIZED_POINT && m_ptWindow.y == WINDOWS_MINIMIZED_POINT)
        return true;

    return false;
};

bool LayeredWindowWrap::Commit()
{
    // ��Ҫ��Ϊ�˷�ֹ�ڷֲ㴰�ڴ�С�ı�ʱ����Ҫ���´������棬
    // �ڻ�������������һ��֮ǰ��ֹ�ύ��������
    if (!m_pWindow->CanRedraw())  
        return true; 

    POINT ptMemDC  = {0,0};
    int   nFlag = ULW_ALPHA/*ULW_OPAQUE*/;
    DWORD dwColorMask = 0;

    BLENDFUNCTION bf;
    bf.BlendOp     = AC_SRC_OVER ;
    bf.AlphaFormat = AC_SRC_ALPHA;        // AC_SRC_ALPHA �ᵼ�´��ڱ�§��,AC_SRC_OVER��ʹ��͸��
    bf.BlendFlags  = 0;                   // Must be zero. 
    bf.SourceConstantAlpha = (byte)m_nAlphaValue;         // 0~255 ͸����
    // 
    // 	if (bf.SourceConstantAlpha != 255)
    // 	{
    // 		nFlag &= ~ULW_OPAQUE;
    // 		nFlag |= ULW_ALPHA;
    // 	}
    // [ע]����xp��win7��ʹ���������������£�COLOR�������⣬��˽���ȫ��ʹ��PNG��ʵ��͸��
    // 	else if (m_pWindow->m_eCurrentTransparentMode & WINDOW_TRANSPARENT_MODE_MASKCOLOR) 
    // 	{
    // 		if (m_pWindow->m_pColMask)
    // 		{
    // 			dwColorMask = m_pWindow->m_pColMask->m_col;
    // 			nFlag = ULW_COLORKEY;
    // 		}
    // 	}

    // 1. ��С����ҲҪˢ�£������ԭ֮��ᷢ�ֻ��治�����ˡ���Ϊ����ʾ��������һ˲�䣬������С��ǰ�Ļ���
    // 2. ��m_sizeWindow����memDC��Сʱ���ᵼ�»�ͼʧ�ܣ�����ֱ���Ե�ǰ��������һ���ز�0 alpha����ƽ�̣�ƽ�����ΪmemDC��С��������ΪsizeWindow��С
    BOOL  bRet = ::UpdateLayeredWindow(
        m_pWindow->m_hWnd, NULL, 
        IsMinimized() ? NULL : &m_ptWindow, 
        &m_sizeWindow, 
        m_pWindow->m_pRenderChain->GetMemoryDC(), 
        &ptMemDC, dwColorMask, &bf, nFlag); 

    static bool bDebug = false;
    if (bDebug)
    {
        m_pWindow->m_pRenderChain->GetMemoryBuffer()->Save(L"C:\\aaa.png", Gdiplus::ImageFormatPNG);
    }
    if (FALSE == bRet)
    {
        UI_LOG_ERROR(_T("%s UpdateLayeredWindow Failed."), FUNC_NAME);
    }

    return true;
}



//
// ���ڷֲ㴰�ڣ�����ʾ��ʱ����Ҫ���¸���һ�Ρ���Ϊ���ֲ㴰�����ص�ʱ���ڴ�λͼû�еõ����£�invisible false��ֱ��return��
//
// ע��������û��ʹ����ӦWM_SHOWWINDOW��ͨ���ֲ㴰��ˢ�£���Ϊ����ӦWM_SHOWWINDOW��ʱ��IsWindowVisible����FALSE
//     ��˸���OnWindowPosChanged���õ�������ʾ��ʱ����ͨ���ֲ㴰��ˢ��
//   
// LRESULT LayeredWindowWrap::_OnWindowPosChanging( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
// {
//     bHandled = FALSE;
//     LPWINDOWPOS lpWndPos = (LPWINDOWPOS)lParam;
// 
//     if (m_pLayeredWindowWrap)
//     {
//         m_pLayeredWindowWrap->OnWindowPosChanging(lpWndPos);
//     }
//     return 0;
// }
// void  LayeredWindowWrap::OnWindowPosChanging(LPWINDOWPOS lpWndPos)
// {
//     // 	if (lpWndPos->flags & SWP_SHOWWINDOW)  // ������ʾ����������ʱ��DrawObject��ʧ�ܣ�
//     // 	{
//     // 		this->RedrawObject(m_pWindow, TRUE);
//     // 	}
// }

//
//	֪ͨ�ֲ㴰���µ�λ�úʹ�С
//
LRESULT LayeredWindowWrap::_OnWindowPosChanged( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    bHandled = FALSE;
    LPWINDOWPOS lpWndPos = (LPWINDOWPOS)lParam;
    OnWindowPosChanged(lpWndPos);
    return 0;
}
void  LayeredWindowWrap::OnWindowPosChanged(LPWINDOWPOS lpWndPos)
{
    // ��С���ˣ�������һ��m_ptWindow�����ں����ж��Ƿ�����С�������޸�m_sizeWindow�����ô���
    if (lpWndPos->x == WINDOWS_MINIMIZED_POINT && lpWndPos->y == WINDOWS_MINIMIZED_POINT)
    {
        m_ptWindow.x = WINDOWS_MINIMIZED_POINT;
        m_ptWindow.y = WINDOWS_MINIMIZED_POINT;
    }
    else
    {
        if (!(lpWndPos->flags & SWP_NOMOVE))
        {
            m_ptWindow.x = lpWndPos->x;
            m_ptWindow.y = lpWndPos->y;
        }
        if (!(lpWndPos->flags & SWP_NOSIZE))
        {
            m_sizeWindow.cx = lpWndPos->cx;
            m_sizeWindow.cy = lpWndPos->cy;
        }
        if (lpWndPos->flags & SWP_SHOWWINDOW)  // ������ʾ����������ʱ��DrawObject��ʧ�ܣ�
        {
            ::InvalidateRect(m_pWindow->m_hWnd, NULL, TRUE);
        }
    }
}

LRESULT LayeredWindowWrap::_OnCancelMode( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    bHandled = FALSE;
    OnExitSizeMove();
    return 0;
}


//	
//  ����UpdateLayeredWindow֧�ָ�ƽ���Ĵ�������Ч������˶��ڷֲ㴰�ڵ�����������һ���߼�ʵ�ֵ�
//
void LayeredWindowWrap::OnLButtonDown(UINT nFlags, POINT pt)
{
    UINT nHitTest = m_pWindow->OnHitTest(&pt);

    switch(nHitTest)
    {
    case HTTOPLEFT:
    case HTTOP:
    case HTTOPRIGHT:
    case HTLEFT:
    case HTRIGHT:
    case HTBOTTOMLEFT:
    case HTBOTTOM:
    case HTBOTTOMRIGHT:
        {
            OnLButtonDown(nHitTest);
            SetMsgHandled(TRUE);
        }
        break;

    default:
        {
            SetMsgHandled(FALSE);
        }
        break;
    }
}

void  LayeredWindowWrap::Enable(bool b)
{
    if (b)
    {
        // ʹ�÷ֲ㴰�ڵĻ����Ͳ�Ҫ����ʾ������Ӱ�ˡ����������˵�����Ĭ���Ǵ�SHADOW�ģ�ʹ�÷ֲ㴰��ʱ�Ͳ���Ҫ�����Ӱ�ˣ��Լ�ģ�⼴�ɣ�
        SetClassLong(m_hWnd, GCL_STYLE, GetClassLong(m_hWnd, GCL_STYLE)&~CS_DROPSHADOW);
        SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetWindowRgn(m_hWnd, NULL, FALSE);   // ȡ�����ڵ����Σ��ɷֲ㴰���Լ��������ֲ㴰�ڽ����յ�һ��WINDOWPOSCHANGED��Ϣ����SWP_NOSIZE��
        // ��˻���Ҫ���ⷢ��һ�����ڴ�С����Ϣ���߷ֲ㴰�ڵ�ǰ��С
    }
    else
    {
        // ע��ֱ��ȥ��WS_EX_LAYERED���ԣ��ᵼ�´����ȱ�ڣ�Ȼ����ˢ��һ�Ρ�
        //     ���������ȥ���ֲ�����֮��ֱ�ӽ�mem bitmap�������Ȼ��������������������⡣
        //     ͬʱΪ�˽�����������ϵ�������rgn����Ĳ��֣���ȥ���ֲ�����֮ǰ���ȼ���
        //     ���ڵ���״

        m_pWindow->UpdateWindowRgn();
        SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);

        HDC hDC = ::GetDC(m_hWnd);
        ::BitBlt(hDC,0,0, m_pWindow->GetWidth(), m_pWindow->GetHeight(), m_pWindow->m_pRenderChain->GetMemoryDC(), 0,0, SRCCOPY);
        ReleaseDC(m_hWnd, hDC);
    }
}


void*  LayeredWindowWrap::QueryInterface(const IID* pIID)
{
    if (IsEqualIID(*pIID, uiiidof(ILayeredWindowWrap)))  
    {   
        SetMsgHandled(TRUE);
        return (void*)GetILayeredWindowWrap();                 
    }                                              
    SetMsgHandled(FALSE);         
    return 0;
    
}

ILayeredWindowWrap*  LayeredWindowWrap::GetILayeredWindowWrap()
{
    if (!m_pILayeredWindowWrap)
        m_pILayeredWindowWrap = new ILayeredWindowWrap(this);

    return m_pILayeredWindowWrap;
}

byte  LayeredWindowWrap::GetAlpha()
{   
    return m_nAlphaValue;
}
void  LayeredWindowWrap::SetAlpha(byte b, bool bUpdate)
{
    m_nAlphaValue = b;
    if (bUpdate)
    {
        this->Commit();
    }
}
}