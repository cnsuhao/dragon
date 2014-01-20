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
	m_bNeedToSetWindowRgn = true;   // 首次显示时，需要设置一下窗口形状

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
    if (this->IsTransparent())  //  例如多层渲染的控件层，需要被alphablend到缓冲上
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
// 屏蔽WM_NCACTIVATE消息，否则在win7上被激活或失活会有边框(但是因为现在不使用WS_THICKFRAME来实现拖拽，也没有这个
// 问题了，这里响应该消息仅为了实现窗口刷新）
// 20121127 后注:强制返回1将导致从一个模态customwindow中再弹出一个模态窗口时（如CFileDialog），新弹出的窗口将永远不能被激活。
//               貌似是子模态窗口会向父窗口发送这个消息进行检测
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

	m_bNeedToSetWindowRgn = true;  // 换肤时，重新更新窗口形状
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
    m_pIMapAttributeRemain = pMapAttrib;  // -- 临时为TransparentMode提供
    {
        WINDOW_TRANSPARENT_MODE eMode = GetTransparentModeTypeFromAttr(pMapAttrib);
        IWndTransMode*  pMode = CreateTransparentModeByType(eMode);
        if (pMode)
        {
            SetWndTransMode(pMode);
        }
    }
    m_pIMapAttributeRemain = pOldMapAttr;

	// 注：这里需要将设置分层窗口的属性放在前面。因为object中很多背景图依赖于窗口类型
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
        pOldMode->Enable(false);  // 注：对于分层窗口的disable，需要先让当前的trans mode设置好它的rgn后，才能取消WS_EX_LAYERED样式，否则会导致窗口变黑
        SAFE_RELEASE(pOldMode);
    }

    m_pRenderChain->GetWindowLayer()->ReCreateRenderTarget();

    // 每次是否需要清空缓存，避免alpha叠加
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
// 1. 实现EraseBkgnd之后，UpdateWindowRgn
// 2. 另外由于一些情况下隐藏时也可能需要绘制（如分层窗口...)
bool CustomWindow::DrawObject(IRenderTarget* pRenderTarget, RenderContext roc)
{
    //if (this->IsVisible())  // 由于分层窗口在下次显示时不会先更新再显示，导致隐藏时作的刷新都没处理。因此将这里的隐藏去掉
    {
        ::UISendMessage(this->GetIMessage(), WM_ERASEBKGND, (WPARAM)pRenderTarget, (LPARAM)&roc );  // 将lparam置为1，与原始消息进行区分

        // 重新设置窗口透明形状 
        // 注：
        //    1. 不将该段代码放在OnErasebkgnd中的原因是，刷新一个按钮时也会走到
        // OnEraseBkgnd中，导致这时的背景图片中被剪裁只剩下一个控件，update window rgn错误。
        //    2. 同理，也不能放在CommitDoubleBuffet2Window中，因为也有可能是初始中刷新了一个对象
        //    3. 如果放在_OnPaint当中的话，则会将窗口上的文字由于采用GDI绘制alpha变成0，也会被抠掉

        if (m_bNeedToSetWindowRgn)
        {
            this->UpdateWindowRgn();
        }

        this->_drawNcChildObject(pRenderTarget, roc);

        roc.DrawClient(m_pIObject);
        roc.Scroll(m_pIObject);
        roc.Update(pRenderTarget);

        ::UISendMessage(this, WM_PAINT, (WPARAM)pRenderTarget, (LPARAM)&roc);       // 将lparam置为1，与原始消息进行区分
        this->DrawChildObject(pRenderTarget, roc);
    }

    return true;
}


// 屏蔽WM_PAINT消息，不需要绘制Nc (否则在拉伸CustomExWindow的时候会出现thickframe )
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
//	获取一个POINT在CustomWindow上面的位置
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
//	注：这个消息其它有可能是在MOUSEMOVE之后手动POST出来的一个消息
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
//	使用通过在 NCHitTest中返回HTCAPTION也能使用点击拖拽的效果，但缺点是会一直产生一个WM_MOUSELEAVE消息，因为鼠标不在客户区域了
//
//	使用WS_THICKFRAME也可以直接实现边框拖拽效果，但还需要去响应NCCACLSIZE和添加一个属性，可能会导致窗口在WIN7下面显示透明边框
//	
//  另外UpdateLayeredWindow支持更平滑的窗口拉伸效果，因此对于分层窗口的拉伸是另外一套逻辑实现的
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


// 注：不要响应UIMSG的WM_SIZE。因为在WindowBase::_OnSize中就已经开始更新窗口了，因
//     此需要在那之前将m_bNeedToSetWindowRgn标志置上。否则将光置上标志，却错过了OnEndEraseBknd
LRESULT  CustomWindow::_OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	// 最小化和从最小化还原时，不需要重新计算窗口的形状
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
		if (lpPos->flags & SWP_FRAMECHANGED) // 第一次最大化，而不是最大化之后所触发的WINDOWPOSCHANGE
		{
			HMONITOR hMonitorTo = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTOPRIMARY);
			if (hMonitorTo != GetPrimaryMonitor())
			{
				// 解决无边框窗口在双屏下面（副屏分辨率大于主屏）时，最大化不正确的问题
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
//	设置窗口是否可以拖拽
//
void CustomWindow::SetResizeCapability( UINT nType )
{
	m_nResizeCapability = nType;
}

//
// 判断窗口的拉伸状态位
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
//	设置/取消一个窗口为分层窗口
//
//	Remark
//		如果一个窗口需要在分层与非分层之间进行切换的话，则必须响应 UI_WM_GET_GRAPHICS_RENDER_LIBRARY_TYPE 消息，
//		并返回RENDER_TYPE_GDIPLUS类型。因为目前不支持切换为分层窗口时，将该窗口中所使用到的
//		图片、字体资源都同时转换成GDIPLUS类型的。
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

// -> 注：每次强制返回HTCAPTION之后，将导致生成WM_MOUSELEAVE消息（鼠标进入非客户区域 )
// 当鼠标不再窗口控件上的位置拖动时候，默认拖动窗口
//LRESULT  CustomWindow::OnNcHitTest( POINT pt )
//{
// 	if (NULL == MouseManager::GetObjectByPos( this->GetWindowObject(), &pt))
// 	{
// 		__super::SetMsgHandled(TRUE);
// 		return HTCAPTION;
// 	}
// 	return HTCLIENT;
//}


/*  注：Removed. 改进：不再通过使用OnNcHitTest返回HTCAPTION来实现拖拽
	目的：在使用CustomWindow/CustomExWindow的时候，如果鼠标拖动的client区域没有其他控件
	      则 OnNcHitTest 会返回 HTCAPTION 。
		  这样就会产生一个问题：返回HTCAPTION之后，就不会产生 WM_MOUSEMOVE消息，而是会
		  产生 WM_NCMOUSEMOVE 消息，这就给 MouseManager 在处理 WM_MOUSEMOVE 消息时带来
		  了错误。因此有必要在 Custom(Ex)Window中将 WM_NCXXX消息再转换成 WM_XXX消息

          另外，通过HITTEST返回HTxxx进行大小改变时，如果窗口有系统菜单样式（WS_SYSMENU），
          则还要继承判断一下当前的系统菜单样式对应的菜单项是否可用（如”大小“被Disable时），
          就算返回了HT_LEFT/HT_TOP，也不能进行拖拽。（Dwp.c (private\ntos\w32\ntuser\kernel)
          Line 613）通常如果窗口没有ThickFrame，该菜单项不可用
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