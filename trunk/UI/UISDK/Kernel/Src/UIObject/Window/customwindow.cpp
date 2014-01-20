#include "stdafx.h"
#include "customwindow.h"
#include "UISDK\Kernel\Src\Renderbase\renderbase\renderbase.h"
#include "UISDK\Kernel\Inc\Interface\imapattr.h"
#include "UISDK\Kernel\Src\RenderLayer\renderchain.h"
#include "UISDK\Kernel\Src\UIObject\Window\wndtransmode\layered\layeredwrap.h"
#include "UISDK\Kernel\Src\UIObject\Window\wndtransmode\aero\aerowrap.h"

CustomWindow::CustomWindow()
{
    m_pTransparentMode = NULL;
    m_pICustomWindow = NULL;
	m_bNeedToSetWindowRgn = true;   // �״���ʾʱ����Ҫ����һ�´�����״

	m_nResizeBorder = 6;
    m_nMaximizeBorder = 3;
	m_nResizeCapability = WRSB_CAPTION;
}
CustomWindow::~CustomWindow()
{
	SAFE_RELEASE(m_pTransparentMode);
}

BOOL  CustomWindow::PreCreateWindow(CREATESTRUCT* pcs)
{
    __super::nvProcessMessage(GetCurMsg(), 0, 0);
    if (FALSE == GetCurMsg()->lRet)
        return FALSE;

	pcs->style = DS_SETFONT | WS_POPUP | WS_SYSMENU | WS_CLIPCHILDREN;
	return TRUE;
}

void CustomWindow::OnInnerInitWindow( )
{
	Window::OnInnerInitWindow();

	LONG dwStyleEx = GetWindowLong(m_hWnd, GWL_EXSTYLE );
	dwStyleEx &= ~ WS_EX_WINDOWEDGE;
	SetWindowLong( m_hWnd, GWL_EXSTYLE, dwStyleEx );

}

GRAPHICS_RENDER_LIBRARY_TYPE  CustomWindow::GetGraphicsRenderType()
{
    if (this->IsTransparent())  //  ��������Ⱦ�Ŀؼ��㣬��Ҫ��alphablend��������
        return GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS;

    if (m_pTransparentMode)
    {
        WINDOW_TRANSPARENT_MODE eMode = m_pTransparentMode->GetModeValue();
        if (eMode== WINDOW_TRANSPARENT_MODE_AERO || eMode == WINDOW_TRANSPARENT_MODE_LAYERED)
             return GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS;;
    }

    return GRAPHICS_RENDER_LIBRARY_TYPE_GDI; 
}

//
WINDOW_TRANSPARENT_MODE  CustomWindow::GetWndTransMode()
{
    if (m_pTransparentMode)
        return m_pTransparentMode->GetModeValue();
    else
        return WINDOW_TRANSPARENT_MODE_NORMAL;
}

LRESULT CustomWindow::_OnNcDestroy( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	return 0;
}


//
// ����WM_NCACTIVATE��Ϣ��������win7�ϱ������ʧ����б߿�(������Ϊ���ڲ�ʹ��WS_THICKFRAME��ʵ����ק��Ҳû�����
// �����ˣ�������Ӧ����Ϣ��Ϊ��ʵ�ִ���ˢ�£�
// 20121127 ��ע:ǿ�Ʒ���1�����´�һ��ģ̬customwindow���ٵ���һ��ģ̬����ʱ����CFileDialog�����µ����Ĵ��ڽ���Զ���ܱ����
//               ò������ģ̬���ڻ��򸸴��ڷ��������Ϣ���м��
//
// Remark
//	Return Nonzero if Windows should proceed with default processing; 0 to prevent the caption bar or icon from being deactivated.
//
LRESULT CustomWindow::_OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	this->SetActive(wParam?true:false);

	bHandled = TRUE;
	return 1;

// 	this->UpdateObject();
// 	return 1;
}

void CustomWindow::ResetAttribute()
{
	__super::ResetAttribute();

	m_bNeedToSetWindowRgn = true;  // ����ʱ�����¸��´�����״
	if (m_pTransparentMode)
	{
		m_pTransparentMode->Enable(false);
		SAFE_RELEASE(m_pTransparentMode);
	}
	m_nResizeBorder = 6;
    m_nMaximizeBorder = 3;
}
void CustomWindow::SetAttribute(IMapAttribute* pMapAttrib, bool bReload )
{
	IMapAttribute* pOldMapAttr = m_pIMapAttributeRemain;
    m_pIMapAttributeRemain = pMapAttrib;  // -- ��ʱΪTransparentMode�ṩ
    {
        WINDOW_TRANSPARENT_MODE eMode = GetTransparentModeTypeFromAttr(pMapAttrib);
        IWndTransMode*  pMode = CreateTransparentModeByType(eMode);
        if (pMode)
        {
            SetWndTransMode(pMode);
        }
    }
    m_pIMapAttributeRemain = pOldMapAttr;

	// ע��������Ҫ�����÷ֲ㴰�ڵ����Է���ǰ�档��Ϊobject�кܶ౳��ͼ�����ڴ�������
	Window::SetAttribute(pMapAttrib, bReload);

    pMapAttrib->GetAttr_int(XML_WINDOW_RESIZE_BORDER, true, &m_nResizeBorder);	
    pMapAttrib->GetAttr_int(XML_WINDOW_MAXIMIZE_BORDER, true, &m_nMaximizeBorder);	
    
    const TCHAR* szText = pMapAttrib->GetAttr(XML_WINDOW_RESIZE_CAPABILITY, true);
    if (szText)
    {
        if (0 == _tcscmp(szText, XML_WINDOW_RESIZE_CAPABILITY_ALL))
        {
            SetResizeCapability(WRSB_ALL);
        }
        else if (0 == _tcscmp(szText, XML_WINDOW_RESIZE_CAPABILITY_CAPTION))
        {
            SetResizeCapability(WRSB_CAPTION);
        }
        else
        {
            SetResizeCapability(WRSB_NONE);
        }
    }
}


void  CustomWindow::OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData)
{
    __super::OnEditorGetAttrList(pData);

    IUIEditor* pEditor = pData->pEditor;
    const TCHAR* szPrefix = pData->szPrefix;

    IUIEditorGroupAttribute*  pWindowGroup = pEditor->CreateGroupAttribute(pData->pGroupAttr, CustomWindow::GetXmlName(), NULL);
    pEditor->CreateComboBoxAttribute(pWindowGroup, XML_WINDOW_TRANSPARENT_TYPE, szPrefix)
        ->AddOption(L"", L"")
        ->AddOption(XML_WINDOW_TRANSPARENT_TYPE_LAYERED)
        ->AddOption(XML_WINDOW_TRANSPARENT_TYPE_AERO)
        ->AddOption(XML_WINDOW_TRANSPARENT_TYPE_MASKALPHA)
        ->AddOption(XML_WINDOW_TRANSPARENT_TYPE_MASKCOLOR);

//    pEditor->CreateTextAttribute(pWindowGroup, XML_WINDOW_TRANSPARENT_TYPE_MASKCOLOR_VALUE, szPrefix);
//    pEditor->CreateTextAttribute(pWindowGroup, XML_WINDOW_TRANSPARENT_TYPE_MASKALPHA_VALUE, szPrefix);
//    pEditor->CreateTextAttribute(pWindowGroup, XML_WINDOW_TRANSPARENT_PART, szPrefix);
//    pEditor->CreateTextAttribute(pWindowGroup, XML_WINDOW_TRANSPARENT_9REGION, szPrefix);
}

void  CustomWindow::SetWndTransMode(WINDOW_TRANSPARENT_MODE eMode, bool bRedraw)
{
	IWndTransMode*  pMode = CreateTransparentModeByType(eMode);
	if (!pMode)
		return;

	SetWndTransMode(pMode);
    if (bRedraw && m_pRenderChain)
    {
        m_pRenderChain->OnWindowPaint(NULL);
    }
}
void  CustomWindow::SetWndTransMode(IWndTransMode* pMode)
{
    if (pMode == m_pTransparentMode)
        return;

    IWndTransMode* pOldMode = m_pTransparentMode;
    m_pTransparentMode = pMode;

	if (pMode)
	{
		pMode->Init(m_pICustomWindow);
		pMode->Enable(true);
	}
    m_bNeedToSetWindowRgn = true;
    if (pOldMode)
    {
        pOldMode->Enable(false);  // ע�����ڷֲ㴰�ڵ�disable����Ҫ���õ�ǰ��trans mode���ú�����rgn�󣬲���ȡ��WS_EX_LAYERED��ʽ������ᵼ�´��ڱ��
        SAFE_RELEASE(pOldMode);
    }

    m_pRenderChain->GetWindowLayer()->ReCreateRenderTarget();

    // ÿ���Ƿ���Ҫ��ջ��棬����alpha����
    WINDOW_TRANSPARENT_MODE eMode = GetWndTransMode();
    if (eMode == WINDOW_TRANSPARENT_MODE_LAYERED || 
        eMode == WINDOW_TRANSPARENT_MODE_AERO)
        this->SetTransparent(true); 
    else
        this->SetTransparent(false);

    if (m_pRenderChain)
    {
        m_pRenderChain->OnWindowTransparentModeChanged(m_pTransparentMode);
    }
}

// [Virtual]
// 1. ʵ��EraseBkgnd֮��UpdateWindowRgn
// 2. ��������һЩ���������ʱҲ������Ҫ���ƣ���ֲ㴰��...)
bool CustomWindow::DrawObject(IRenderTarget* pRenderTarget, RenderContext roc)
{
    //if (this->IsVisible())  // ���ڷֲ㴰�����´���ʾʱ�����ȸ�������ʾ����������ʱ����ˢ�¶�û������˽����������ȥ��
    {
        ::UISendMessage(this->GetIMessage(), WM_ERASEBKGND, (WPARAM)pRenderTarget, (LPARAM)&roc );  // ��lparam��Ϊ1����ԭʼ��Ϣ��������

        // �������ô���͸����״ 
        // ע��
        //    1. �����öδ������OnErasebkgnd�е�ԭ���ǣ�ˢ��һ����ťʱҲ���ߵ�
        // OnEraseBkgnd�У�������ʱ�ı���ͼƬ�б�����ֻʣ��һ���ؼ���update window rgn����
        //    2. ͬ��Ҳ���ܷ���CommitDoubleBuffet2Window�У���ΪҲ�п����ǳ�ʼ��ˢ����һ������
        //    3. �������_OnPaint���еĻ�����Ὣ�����ϵ��������ڲ���GDI����alpha���0��Ҳ�ᱻ�ٵ�

        if (m_bNeedToSetWindowRgn)
        {
            this->UpdateWindowRgn();
        }

        this->_drawNcChildObject(pRenderTarget, roc);

        roc.DrawClient(m_pIObject);
        roc.Scroll(m_pIObject);
        roc.Update(pRenderTarget);

        ::UISendMessage(this, WM_PAINT, (WPARAM)pRenderTarget, (LPARAM)&roc);       // ��lparam��Ϊ1����ԭʼ��Ϣ��������
        this->DrawChildObject(pRenderTarget, roc);
    }

    return true;
}


// ����WM_PAINT��Ϣ������Ҫ����Nc (����������CustomExWindow��ʱ������thickframe )
LRESULT CustomWindow::_OnNcPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;
	return 0;
}

void CustomWindow::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
	if (m_pBkgndRender || m_pForegndRender || m_pMaterialRender)
	{
		CRect rc(0,0, GetWidth(), GetHeight());

		int nState = this->IsActive()?WINDOW_BKGND_RENDER_STATE_ACTIVE:WINDOW_BKGND_RENDER_STATE_INACTIVE;

        if (m_pBkgndRender)
        {
            CRect rcBkgnd(&rc);
            rcBkgnd.DeflateRect(&m_rcBkgndRenderRegion);
            m_pBkgndRender->DrawState(pRenderTarget, &rcBkgnd, nState);
        }

        if (m_pMaterialRender)
        {
            CRect rcTextureRegion(&rc);
            rcTextureRegion.DeflateRect(&m_rcMaterialRenderRegion);
            m_pMaterialRender->DrawState(pRenderTarget, &rcTextureRegion, nState);
        }

        if (m_pForegndRender)
        {
            CRect rcForegnd(&rc);
            rcForegnd.DeflateRect(&m_rcForegndRenderRegion);
            m_pForegndRender->DrawState(pRenderTarget, &rcForegnd, nState);
        }
	}
	else
	{
		__super::OnEraseBkgnd(pRenderTarget);
	}
}

void CustomWindow::UpdateWindowRgn()
{
	this->m_bNeedToSetWindowRgn = false;

    if (m_pTransparentMode)
        m_pTransparentMode->UpdateRgn();
}


void CustomWindow::CommitDoubleBuffet2Window(HDC hDCWnd, RECT* prcCommit, int nRectCount)
{
	if (m_pTransparentMode && m_pTransparentMode->Commit())
        return;
    
    __super::CommitDoubleBuffet2Window(hDCWnd, prcCommit, nRectCount);		
}

//
//	��ȡһ��POINT��CustomWindow�����λ��
//
UINT CustomWindow::OnHitTest(POINT* pt)
{
	if (m_nResizeCapability == WRSB_NONE)
	{
		return HTCLIENT;
	}
 	else if (m_nResizeCapability == WRSB_CAPTION)
	{
		if (NULL == GetHoverObject() && NULL == GetPressObject()  && !IsZoomed(m_hWnd))
			return HTCAPTION;
	}

	if (NULL == pt)
		return HTERROR;

	UINT nHitTest = HTCLIENT;
	if (IsZoomed(m_hWnd))
		return HTCLIENT;

	RECT rc;
	::GetClientRect( m_hWnd, &rc );
	int nWidth  = rc.right - rc.left;
	int nHeight = rc.bottom - rc.top;

	if (pt->x < m_nResizeBorder)
	{
		if (pt->y < m_nResizeBorder && this->TestResizeBit(WRSB_TOPLEFT))
			nHitTest = HTTOPLEFT;
		else if (pt->y > nHeight - m_nResizeBorder  && this->TestResizeBit(WRSB_BOTTOMLEFT))
			nHitTest = HTBOTTOMLEFT;
		else if (this->TestResizeBit(WRSB_LEFT))
			nHitTest = HTLEFT;
	}
	else if (pt->x >= nWidth-m_nResizeBorder)
	{
		if (pt->y < m_nResizeBorder && this->TestResizeBit(WRSB_TOPRIGHT))
			nHitTest = HTTOPRIGHT;
		else if (pt->y > nHeight - m_nResizeBorder && this->TestResizeBit(WRSB_BOTTOMRIGHT))
			nHitTest = HTBOTTOMRIGHT;
		else if (this->TestResizeBit(WRSB_RIGHT))
			nHitTest = HTRIGHT;
	}
	else if (pt->y < m_nResizeBorder && this->TestResizeBit(WRSB_TOP))
	{
		nHitTest = HTTOP;
	}
	else if (pt->y >= nHeight - m_nResizeBorder && this->TestResizeBit(WRSB_BOTTOM))
	{
		nHitTest = HTBOTTOM;
	}
 	else
 	{
 		if (m_nResizeCapability & WRSB_CAPTION && NULL == GetHoverObject() && NULL == GetPressObject() && !IsZoomed(m_hWnd))
 		{
 			nHitTest = HTCAPTION;
 		}
 	}

	return nHitTest;

}

//
//	ע�������Ϣ�����п�������MOUSEMOVE֮���ֶ�POST������һ����Ϣ
//
BOOL CustomWindow::OnSetCursor( HWND hWnd, UINT nHitTest, UINT message )
{
	POINT pt;
	::GetCursorPos(&pt);
	::MapWindowPoints(NULL,m_hWnd,&pt,1);
	nHitTest = this->OnHitTest(&pt);
	switch(nHitTest)
	{
	case HTTOPLEFT:
	case HTBOTTOMRIGHT:
		::SetCursor( ::LoadCursor(NULL, IDC_SIZENWSE) );
		break;

	case HTTOP:
	case HTBOTTOM:
		::SetCursor( ::LoadCursor(NULL, IDC_SIZENS) );
		break;

	case HTLEFT:
	case HTRIGHT:
		::SetCursor( ::LoadCursor(NULL, IDC_SIZEWE) );
		break;

	case HTTOPRIGHT:
	case HTBOTTOMLEFT:
		::SetCursor( ::LoadCursor(NULL, IDC_SIZENESW) );
		break;

	case HTCAPTION:
		::SetCursor( ::LoadCursor(NULL, IDC_ARROW) );
		break;

	default:
		::SetCursor( ::LoadCursor(NULL, IDC_ARROW) );
		break;	
	}
	
	return TRUE;
}

//
//	ʹ��ͨ���� NCHitTest�з���HTCAPTIONҲ��ʹ�õ����ק��Ч������ȱ���ǻ�һֱ����һ��WM_MOUSELEAVE��Ϣ����Ϊ��겻�ڿͻ�������
//
//	ʹ��WS_THICKFRAMEҲ����ֱ��ʵ�ֱ߿���קЧ����������Ҫȥ��ӦNCCACLSIZE�����һ�����ԣ����ܻᵼ�´�����WIN7������ʾ͸���߿�
//	
//  ����UpdateLayeredWindow֧�ָ�ƽ���Ĵ�������Ч������˶��ڷֲ㴰�ڵ�����������һ���߼�ʵ�ֵ�
//
void CustomWindow::OnLButtonDown(UINT nFlags, POINT pt)
{
	SetMsgHandled(FALSE);

	UINT nHitTest = this->OnHitTest(&pt);
	switch(nHitTest)
	{
	case HTTOPLEFT:
		::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_TOPLEFT, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTTOP:
		::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_TOP, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTTOPRIGHT:
		::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_TOPRIGHT, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTLEFT:
		::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_LEFT, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTRIGHT:
		::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_RIGHT, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTBOTTOMLEFT:
		::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_BOTTOMLEFT, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTBOTTOM:
		::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_BOTTOM, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTBOTTOMRIGHT:
	    ::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_BOTTOMRIGHT, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTCAPTION:
		::PostMessage( m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(pt.x,pt.y) );
		break;
	}
}


// ע����Ҫ��ӦUIMSG��WM_SIZE����Ϊ��WindowBase::_OnSize�о��Ѿ���ʼ���´����ˣ���
//     ����Ҫ����֮ǰ��m_bNeedToSetWindowRgn��־���ϡ����򽫹����ϱ�־��ȴ�����OnEndEraseBknd
LRESULT  CustomWindow::_OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	// ��С���ʹ���С����ԭʱ������Ҫ���¼��㴰�ڵ���״
	if (SIZE_MINIMIZED == wParam || (SIZE_RESTORED==wParam && ::IsIconic(m_hWnd)))
		return 0;

	this->m_bNeedToSetWindowRgn = true;
    return 0;
}

LRESULT CustomWindow::_OnWindowPosChanging( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
    bHandled = FALSE;
    if (IsZoomed(m_hWnd))
    {
        LPWINDOWPOS lpPos = (LPWINDOWPOS)lParam;
		if (lpPos->flags & SWP_FRAMECHANGED) // ��һ����󻯣����������֮����������WINDOWPOSCHANGE
		{
			HMONITOR hMonitorTo = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY);
			if (hMonitorTo != GetPrimaryMonitor())
			{
				// ����ޱ߿򴰿���˫�����棨�����ֱ��ʴ���������ʱ����󻯲���ȷ������
				MONITORINFO  miTo = {sizeof(miTo), 0};
				GetMonitorInfo(hMonitorTo, &miTo);

				lpPos->x = miTo.rcWork.left;
				lpPos->y = miTo.rcWork.top;
				lpPos->cx = RECTW(miTo.rcWork);
				lpPos->cy = RECTH(miTo.rcWork);
			}


			lpPos->x -= m_nMaximizeBorder;
			lpPos->y -= m_nMaximizeBorder;
			lpPos->cx += m_nMaximizeBorder*2;
			lpPos->cy += m_nMaximizeBorder*2;
		}
    }
    return 0;
}

//
//	���ô����Ƿ������ק
//
void CustomWindow::SetResizeCapability( UINT nType )
{
	m_nResizeCapability = nType;
}

//
// �жϴ��ڵ�����״̬λ
//
bool  CustomWindow::TestResizeBit( int nBit )
{
	if (m_nResizeCapability & nBit)
		return true;
	else 
		return false;
}


bool  CustomWindow::IsWindowLayered()
{
    if (m_pTransparentMode && WINDOW_TRANSPARENT_MODE_LAYERED == m_pTransparentMode->GetModeValue())
        return true;

    return false;
}

//
//	����/ȡ��һ������Ϊ�ֲ㴰��
//
//	Remark
//		���һ��������Ҫ�ڷֲ���Ƿֲ�֮������л��Ļ����������Ӧ UI_WM_GET_GRAPHICS_RENDER_LIBRARY_TYPE ��Ϣ��
//		������RENDER_TYPE_GDIPLUS���͡���ΪĿǰ��֧���л�Ϊ�ֲ㴰��ʱ�����ô�������ʹ�õ���
//		ͼƬ��������Դ��ͬʱת����GDIPLUS���͵ġ�
//
void  CustomWindow::EnableWindowLayered(bool b)
{
    bool bLayered = IsWindowLayered();
	if (b == bLayered)
		return;

	if (b)
	{
		IWndTransMode* pMode = static_cast<IWndTransMode*>(new LayeredWindowWrap());
        SetWndTransMode(pMode);
	}
	else
	{
        m_pTransparentMode->Enable(false);
        SetWndTransMode(NULL);
	}
}


void  CustomWindow::EnableWindowAero(bool b)
{
    bool bAero = (m_pTransparentMode && WINDOW_TRANSPARENT_MODE_AERO == m_pTransparentMode->GetModeValue());
    if (b == bAero)
        return;

    if (b)
    {
        IWndTransMode* pMode = static_cast<IWndTransMode*>(new AeroWindowWrap());
        SetWndTransMode(pMode);
    }
    else
    {
        SetWndTransMode(NULL);
    }
}

// -> ע��ÿ��ǿ�Ʒ���HTCAPTION֮�󣬽���������WM_MOUSELEAVE��Ϣ��������ǿͻ����� )
// ����겻�ٴ��ڿؼ��ϵ�λ���϶�ʱ��Ĭ���϶�����
//LRESULT  CustomWindow::OnNcHitTest( POINT pt )
//{
// 	if (NULL == MouseManager::GetObjectByPos( this->GetWindowObject(), &pt))
// 	{
// 		__super::SetMsgHandled(TRUE);
// 		return HTCAPTION;
// 	}
// 	return HTCLIENT;
//}


/*  ע��Removed. �Ľ�������ͨ��ʹ��OnNcHitTest����HTCAPTION��ʵ����ק
	Ŀ�ģ���ʹ��CustomWindow/CustomExWindow��ʱ���������϶���client����û�������ؼ�
	      �� OnNcHitTest �᷵�� HTCAPTION ��
		  �����ͻ����һ�����⣺����HTCAPTION֮�󣬾Ͳ������ WM_MOUSEMOVE��Ϣ�����ǻ�
		  ���� WM_NCMOUSEMOVE ��Ϣ����͸� MouseManager �ڴ��� WM_MOUSEMOVE ��Ϣʱ����
		  �˴�������б�Ҫ�� Custom(Ex)Window�н� WM_NCXXX��Ϣ��ת���� WM_XXX��Ϣ

          ���⣬ͨ��HITTEST����HTxxx���д�С�ı�ʱ�����������ϵͳ�˵���ʽ��WS_SYSMENU����
          ��Ҫ�̳��ж�һ�µ�ǰ��ϵͳ�˵���ʽ��Ӧ�Ĳ˵����Ƿ���ã��硱��С����Disableʱ����
          ���㷵����HT_LEFT/HT_TOP��Ҳ���ܽ�����ק����Dwp.c (private\ntos\w32\ntuser\kernel)
          Line 613��ͨ���������û��ThickFrame���ò˵������
*/

// void CustomWindow::OnNcMouseMove( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_MOUSEMOVE, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcMouseHover( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_MOUSEHOVER, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcMouseLeave()
// {
// 	SetMsgHandled(FALSE);
// 	::SendMessage( m_hWnd, WM_MOUSELEAVE, (WPARAM)0, (LPARAM)0 );
// }
// void CustomWindow::OnNcLButtonDown( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_LBUTTONDOWN, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcLButtonUp( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_LBUTTONUP, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcLButtonDblClk( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_LBUTTONDBLCLK, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcRButtonDown( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_RBUTTONDOWN, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcRButtonUp( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_RBUTTONUP, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcRButtonDblClk( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_RBUTTONDBLCLK, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcMButtonDown( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_MBUTTONDOWN, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcMButtonUp( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_MBUTTONUP, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcMButtonDblClk( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_MBUTTONDBLCLK, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }


//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////