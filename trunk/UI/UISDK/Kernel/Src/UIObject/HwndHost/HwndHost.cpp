#include "stdafx.h"
#include "HwndHost.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"

HwndHost::HwndHost()
{
    m_pIHwndHost = NULL;
	m_hWnd = NULL;
	m_oldWndProc  = NULL;
}
HwndHost::~HwndHost()
{
	Detach();
	UIASSERT ( NULL == m_oldWndProc );
}

// 
//	���໯���ڣ��滻���Ĵ��ڹ���
//
//	Return
//		�ɹ�����false��ʧ�ܷ���false
//
bool HwndHost::SubclassWindow()
{
	UIASSERT( m_hWnd != NULL );

	this->m_thunk.Init( &HwndHost::_WndProc, this );
	WNDPROC pProc = this->m_thunk.GetWNDPROC();
	this->m_oldWndProc = (WNDPROC)(LONG_PTR) ::SetWindowLong( m_hWnd, GWLP_WNDPROC, (LONG)(LONG_PTR)pProc);

	this->OnSubclassWindow();

	return true;
}

// 
//	ȡ�����໯���ڣ��ָ����Ĵ��ڹ���
//
//	Return
//		�ɹ�����false��ʧ�ܷ���false
//
bool HwndHost::UnSubclassWindow()
{
	::SetWindowLong( m_hWnd, GWLP_WNDPROC, (LONG)(LONG_PTR)m_oldWndProc);
	m_oldWndProc = NULL;

	this->OnUnsubclassWindow();
	return true;
}

void HwndHost::OnSubclassWindow()
{
    UpdateTabStopStyle();
}

void HwndHost::UpdateTabStopStyle()
{
    long lStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
    if (lStyle & WS_TABSTOP)
    {
        m_nStyle |= CONTROL_STYLE_TABSTOP;
    }
    else
    {
        m_nStyle &= ~CONTROL_STYLE_TABSTOP;
    }
}
void HwndHost::OnUnsubclassWindow()
{
}

void HwndHost::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
	__super::SetAttribute(pMapAttrib, bReload);

	HWND hParentWnd = this->GetHWND();

    int nControlID = 0;
    if (MAPATTR_RET_NOT_EXIST != pMapAttrib->GetAttr_int(XML_HWNDHOST_CONTROL_ID, true, &nControlID))
	{
		HWND hWnd = ::GetDlgItem(hParentWnd, nControlID);
		if (NULL == hWnd)
		{
			UI_LOG_WARN(_T("HwndHost::SetAttribute Failed. id=%s, GetDlgItem(%d)"), m_strId.c_str(), nControlID);
			return ;
		}

		Attach(hWnd);
	}
}

void HwndHost::GetDesiredSize(SIZE* pSize)
{
	pSize->cx = pSize->cy = 0;
	if (m_hWnd)
	{
		CRect rc;
		::GetWindowRect(m_hWnd, &rc);
		pSize->cx = rc.Width();
		pSize->cy = rc.Height();
	}
}

HWND  HwndHost::Detach()
{
	if (!m_hWnd)
		return NULL;

	HWND hWnd = m_hWnd;
	UnSubclassWindow();
	m_hWnd = NULL;
	return hWnd;
}
void  HwndHost::Attach(HWND hWnd)
{
	Detach();
	m_hWnd = hWnd;
	if (m_hWnd)
	{
		this->SubclassWindow();
	}
}

//
//	[static] LRESULT CALLBACK _WndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
//
//	��ATL��thunk�滻���Ĵ��ڹ���
//
//	Parameter��
//		hwnd
//			[in]	�������ڱ��滻���ˣ������hwnd��thisָ��
//
//		uMsg,wParam,lParam
//			[in]	��Ϣ��Ϣ
//
LRESULT  HwndHost::_WndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	HwndHost* pThis = (HwndHost*)hwnd;
	return pThis->WndProc( uMsg, wParam, lParam );
}

LRESULT HwndHost::DefWindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( NULL != m_oldWndProc )
	{
		return ::CallWindowProc( m_oldWndProc, m_hWnd, uMsg, wParam, lParam );
	}

	return 0;
}

//
//	[private] LRESULT WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
//
//	���ڱ����໯��֮��Ĵ��ڹ���
//
LRESULT	HwndHost::WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
	case WM_SETFOCUS:
		{
			this->SetFocus(true);
			WindowBase* pWindow = this->GetWindowObject();
			if (pWindow)
			{
				pWindow->GetMouseMgr()->SetFocusObjectDirect(this);
				::UISendMessage(this, WM_SETFOCUS, 
					(WPARAM)pWindow->GetMouseMgr()->GetOldFocusObject(),
					0 );
			}
		}
		break;

	case WM_KILLFOCUS:
		{
			this->SetFocus(false);
			WindowBase* pWindow = this->GetWindowObject();
			if (pWindow)
			{
				::UISendMessage(this, WM_KILLFOCUS, 
					(WPARAM)pWindow->GetMouseMgr()->GetFocusObject(),
					0 );
			}
		}
		break;

	case WM_NCDESTROY:
		{
			LRESULT lRet = DefWindowProc(uMsg,wParam,lParam);
			this->UnSubclassWindow();  // ����� oldwndproc������ȵ���Ĭ�Ϲ���
			m_hWnd = NULL;
			return lRet;
		}
		break;

    case WM_STYLECHANGED:
        {
            UpdateTabStopStyle();
        }
        break;

    case WM_WINDOWPOSCHANGED:
        {
            UIASSERT(m_pParent);

            if (m_pParent->GetObjectType() == OBJ_WINDOW)  // ֱ�ӷ����˸���������
            {
                ::GetWindowRect(m_hWnd, &m_rcParent);
                HWND hWndParent = ::GetParent(m_hWnd);
                ::MapWindowPoints(NULL, hWndParent, (LPPOINT)&m_rcParent, 2);
            }
            else   // �п����Ƿ�����һ��panel����
            {
                RECT  rcWindow;
                ::GetWindowRect(m_hWnd, &rcWindow);
                HWND hWndParent = m_pIHwndHost->GetHWND();
                ::MapWindowPoints(NULL, hWndParent, (LPPOINT)&rcWindow, 2);

                m_pParent->WindowRect2ObjectClientRect(&rcWindow, &m_rcParent);
            }
        }
        break;
	}
	return DefWindowProc(uMsg,wParam,lParam);
}

UINT HwndHost::OnHitTest(POINT* pt, POINT*  ptInChild)
{
	POINT point = *pt;
	MapWindowPoints(GetHWND(), NULL, &point, 1);
	UINT nRet = ::SendMessage(m_hWnd, WM_NCHITTEST, 0, MAKELPARAM(point.x, point.y));
	return nRet;
}

void HwndHost::OnVisibleChanged(BOOL bVisible, IObject* pObjChanged)
{
    SetMsgHandled(FALSE);

	if (false == this->IsMySelfVisible())
		return;

	if (bVisible)
	{
		// ����ֱ�ӵ���this->IsVisible���ж�IsWindowVisible(m_hWnd)�����µõ���ֵ����ȷ
		// �������������жϸ����ڵĿɼ�
		if (m_pParent && m_pParent->IsVisible())
		{
			ShowWindow(m_hWnd, SW_SHOWNOACTIVATE);
		}
	}
	else
	{
		//if (::IsWindowVisible(m_hWnd)) <-- ע�����ﲻ�ܼ�������жϣ���Ϊ����OnInitDialog������parentʱ��HwndHost
		//                                       ��IsWindowVisible�᷵��FALSE�������жϴ���
		{
			ShowWindow(m_hWnd, SW_HIDE);
		}
	}
}