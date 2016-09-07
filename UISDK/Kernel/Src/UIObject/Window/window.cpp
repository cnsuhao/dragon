#include "stdafx.h"
#include "window.h"
#include "UISDK\Kernel\Src\Base\Application\uiapplication.h"

Window::Window()
{
    m_pIWindow = NULL;
	m_pBtnSystemMaximize = NULL;
	m_pBtnSystemRestore  = NULL;
	m_pBtnSystemMinimize = NULL;
	m_pBtnSystemClose = NULL;
}

Window::~Window(void)
{
}

void Window::OnInnerInitWindow()
{
	this->HandleSysBtnStyle();

	// Ĭ����ʾ��󻯰�ť�����ػ�ԭ��ť
    if (::IsZoomed(m_hWnd))
    {
	    if (m_pBtnSystemMaximize)
		    m_pBtnSystemMaximize->SetVisible(false, false);
    }
    else
    {
        if (m_pBtnSystemRestore)
            m_pBtnSystemRestore->SetVisible(false, false);
    }
}

void Window::_OnSkinChanged()
{
	SetMsgHandled(FALSE);
	this->HandleSysBtnStyle();
}

void Window::HandleSysBtnStyle()
{
    m_pBtnSystemMinimize = NULL;
    m_pBtnSystemRestore = NULL;
    m_pBtnSystemMaximize = NULL;
    m_pBtnSystemClose = NULL;

	Object* p = this->_findChildObjectItem(XML_SYS_BTN_MINIMIZE, true);
    if (p)
        m_pBtnSystemMinimize = p->GetIObject();

	p = this->_findChildObjectItem(XML_SYS_BTN_RESTORE, true);
    if (p)
        m_pBtnSystemRestore = p->GetIObject();

	p = this->_findChildObjectItem(XML_SYS_BTN_MAXIMIZE, true);
    if (p)
        m_pBtnSystemMaximize = p->GetIObject();

	p = this->_findChildObjectItem(XML_SYS_BTN_CLOSE, true);
    if (p)
        m_pBtnSystemClose = p->GetIObject();

	// ȥ����Щ��ť��TABSTOP����
	ObjStyle s = {0};
	s.tabstop = 1;
	if (m_pBtnSystemClose)
		m_pBtnSystemClose->ModifyObjectStyle(0, &s);
	if (m_pBtnSystemRestore)
		m_pBtnSystemRestore->ModifyObjectStyle(0, &s);
	if (m_pBtnSystemMinimize)
		m_pBtnSystemMinimize->ModifyObjectStyle(0, &s);
	if (m_pBtnSystemMaximize)
		m_pBtnSystemMaximize->ModifyObjectStyle(0, &s);

	// ����ϵͳ��ť�����ݾ���������ʽ
	UINT nAdd = 0, nRemove = 0;

	if (m_pBtnSystemMinimize)
	{
		nAdd |= WS_MINIMIZEBOX;
	}
	else
	{
		nRemove |= WS_MINIMIZEBOX;
	}

	if (m_pBtnSystemMaximize)
	{
		nAdd |= WS_MAXIMIZEBOX;
	}
	else
	{
		nRemove |= WS_MAXIMIZEBOX;
	}

	long lStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	lStyle |= nAdd;
	lStyle &= ~nRemove;
	SetWindowLong(m_hWnd, GWL_STYLE, lStyle);
//	SetWindowPos(m_hWnd, 0,0,0,0,0,SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_FRAMECHANGED);
}
void Window::_OnLButtonDblClk(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);
	if (::IsZoomed(m_hWnd))
	{
		OnSysRestore();
	}
	else if (m_pBtnSystemMaximize)
	{
		OnSysMaximize();
	}
}

void  Window::OnSysClose()
{
	::PostMessage(m_hWnd, WM_SYSCOMMAND,SC_CLOSE, 0 );
}
void  Window::OnSysMinimize()
{
	::PostMessage(m_hWnd, WM_SYSCOMMAND,SC_MINIMIZE, 0 );
}
void Window::OnSysMaximize()
{
	::PostMessage(m_hWnd, WM_SYSCOMMAND,SC_MAXIMIZE, 0 );
}
void Window::OnSysRestore()
{
	::PostMessage(m_hWnd, WM_SYSCOMMAND,SC_RESTORE, 0 );
}

// win7�����WS_THICKFRAME��ʽ�����������/��ԭ����Ϣֻ��WM_SIZE��û��WM_SYSCOMMAND
// �������Ͳ���ʹ��WM_SYSCOMMAND��Ϣ������fuck!!
void  Window::virtualOnSize(UINT nType, UINT nWidth, UINT nHeight)
{
    __super::virtualOnSize(nType, nWidth, nHeight);

    if (SIZE_MAXIMIZED == nType)
    {
        // ������ʾ��󻯰�ť����ʾ��ԭ��ť
        if (m_pBtnSystemMaximize)
            m_pBtnSystemMaximize->SetVisible(false, false, false);

        if (m_pBtnSystemRestore)
            m_pBtnSystemRestore->SetVisible(true, true, true);
    }
    else if(SIZE_RESTORED == nType)
    {
        if (m_pBtnSystemRestore && m_pBtnSystemRestore->IsSelfVisible())
        {
            // ������ʾ��ԭ��ť����ʾ��󻯰�ť
            if (m_pBtnSystemRestore)
                m_pBtnSystemRestore->SetVisible(false, false, false);

            if (m_pBtnSystemMaximize)
                m_pBtnSystemMaximize->SetVisible(true, true, false);
        }
    }
}

void  Window::OnClose()
{
    SetMsgHandled(FALSE);  // Ĭ�ϴ�������DestroyWindow
    if (m_bDoModal)
    {
        EndDialog(IDCANCEL);
    }

    if (m_pUIApplication->IsDesignMode())   // �ڱ༭���еĴ���Ԥ��ģʽ���ô������ⲿ����������ʱ֪ͨ�ⲿ�ͷ�
    {
        HWND hNotifyWnd = (HWND) m_pUserData;
        if (::IsWindow(hNotifyWnd))
            ::PostMessage(hNotifyWnd, UI_WM_PREVIEW_WINDOW_DESTROY, (WPARAM)m_pIWindow, 0);
    }
}