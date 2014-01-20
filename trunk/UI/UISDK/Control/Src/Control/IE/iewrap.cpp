#include "stdafx.h"
#include "iewrap.h"
#include "UISDK\Kernel\Inc\Util\rendercontext.h"
#include "UISDK\Kernel\Inc\Util\iimage.h"

namespace UI
{
IEWrap::IEWrap()
{
    m_pIIEWrap = NULL;

	m_pWebBrowser = NULL;
	m_pOleObject = NULL;
	m_pViewObject = NULL;
	m_pSite = NULL;
	m_hWndShellEmbedding = NULL;
    m_hWndShellDocObjectView = NULL;
    m_hWndIE = NULL;
    m_dwEventCookie = 0;
    m_bNeedFixAlpha = false;
    m_bPrintingWindow = false;
    m_oldWndProc = NULL;
}

IEWrap::~IEWrap()
{
    if (m_dwEventCookie && m_pWebBrowser)
    {
        AtlUnadvise(m_pWebBrowser, DIID_DWebBrowserEvents2, m_dwEventCookie);
        m_dwEventCookie = 0;
    }
    if (m_oldWndProc)
    {
        UnSubclassIEWindow();
    }

	if (m_pOleObject)
	{
		m_pOleObject->SetClientSite(NULL);
		m_pOleObject->Release();
	}
	if (m_pViewObject)
	{
		m_pViewObject->SetAdvise(DVASPECT_CONTENT, 0, NULL);
		m_pViewObject->Release();
	}
	SAFE_RELEASE(m_pWebBrowser);
	SAFE_RELEASE(m_pSite);
}


void IEWrap::SetAttribute(IMapAttribute* pMatAttrib, bool bReload)
{
	DO_PARENT_PROCESS(IIEWrap, IHwndHost);

	CreateControl();

	if (!bReload)
	{
		const TCHAR* szText = pMatAttrib->GetAttr(XML_IE_URL, true);
		if (szText)
		{
			if (FAILED(Navigate(szText)))
			{
				UI_LOG_WARN(_T("%s Navigate failed. url=%s"), FUNC_NAME, szText);
			}
		}
	}
}
void IEWrap::CreateControl()
{
	HRESULT hr = S_OK;
	
	do
	{
		hr = ::CoCreateInstance(CLSID_WebBrowser, NULL, CLSCTX_INPROC, IID_IWebBrowser2, (void**)&m_pWebBrowser);

		if (FAILED(hr) || NULL == m_pWebBrowser)
		{
			UI_LOG_WARN(_T("%s Create WebBrowser ActiveX Control Failed."), FUNC_NAME);
			break;
		}

#if 0
        IUIApplication* pUIApp = m_pIIEWrap->GetUIApplication();
		HWND hParentWnd = m_pIIEWrap->GetHWND();
		if (1 || Util::IsLayeredWindow(hParentWnd))
		{
			m_pIIEWrap->RemoveMeInTheTree();

            PopupIEWindow::CreateInstance(pUIApp, &m_pPopupIEWnd);
            m_pPopupIEWnd->m_pIEWrap = this;

 			m_pPopupIEWnd->Create(pUIApp, _T(""), hParentWnd);
			m_pPopupIEWnd->AddChild(m_pIIEWrap);
//            m_pPopupIEWnd->SetObjectPos(0, 0, m_pIIEWrap->GetConfigWidth(), m_pIIEWrap->GetConfigHeight(), SWP_NOREDRAW|SWP_NOMOVE);
			
            IMapAttribute* pEmpty = NULL;
            UICreateIMapAttribute(&pEmpty);

			SERIALIZEDATA sdata = {0};
			sdata.pMapAttrib = pEmpty;
			sdata.nFlag = SERIALIZEFLAG_LOAD;
			UISendMessage(m_pPopupIEWnd, UI_WM_SERIALIZE, (WPARAM)&sdata);
//			UISendMessage(m_pPopupIEWnd, UI_WM_SETATTRIBUTE, (WPARAM)pEmpty, (LPARAM)false);
            SAFE_RELEASE(pEmpty);

			SyncWindowData data;
			data.m_hWnd = m_pPopupIEWnd->GetHWND();
            data.m_nAnchorType = ANCHOR_CUSTOM;
// 
// 			if (m_pIIEWrap->GetConfigLeft() != NDEF)
// 			{
// 				data.m_nAnchorType |= ANCHOR_LEFT;
// 				data.m_rcAnchorData.xOffset = m_pIIEWrap->GetConfigLeft();
// 			}
// 			if (m_pIIEWrap->GetConfigRight() != NDEF)
// 			{
// 				data.m_nAnchorType |= ANCHOR_RIGHT;
// 				if (m_pIIEWrap->GetConfigLeft() == NDEF)
// 					data.m_rcAnchorData.xOffset = m_pIIEWrap->GetConfigRight();
// 				else
// 					data.m_rcAnchorData.xOffset2 = m_pIIEWrap->GetConfigRight();
// 			}
// 			if (m_pIIEWrap->GetConfigTop() != NDEF)
// 			{
// 				data.m_nAnchorType |= ANCHOR_TOP;
// 				data.m_rcAnchorData.yOffset = m_pIIEWrap->GetConfigTop();
// 			}
// 			if (m_pIIEWrap->GetConfigBottom() != NDEF)
// 			{
// 				data.m_nAnchorType |= ANCHOR_BOTTOM;
// 				if (m_pIIEWrap->GetConfigTop() == NDEF)
// 					data.m_rcAnchorData.yOffset = m_pIIEWrap->GetConfigBottom();
// 				else
// 					data.m_rcAnchorData.yOffset2 = m_pIIEWrap->GetConfigBottom();
// 			}
// 			if (m_pIIEWrap->GetConfigWidth() > 0)
// 				data.m_rcAnchorData.Width = m_pIIEWrap->GetConfigWidth();
// 			if (m_pIIEWrap->GetConfigHeight() > 0)
// 				data.m_rcAnchorData.Height = m_pIIEWrap->GetConfigHeight();
            
//            UIASSERT(0 && _T("TODO: 未完成"));

//			::SendMessage(hParentWnd, UI_WM_SYNC_WINDOW, (WPARAM)ADD_SYNC_WINDOW, (LPARAM)&data);
			::ShowWindow(m_pPopupIEWnd->GetHWND(), SW_SHOWNOACTIVATE);

			m_pPopupIEWnd->UpdateLayout(true);
		}
#endif

		hr = m_pWebBrowser->QueryInterface(IID_IOleObject, (void**)&m_pOleObject);
		if (FAILED(hr) || NULL == m_pOleObject)	
			break;

		hr = m_pOleObject->QueryInterface(__uuidof(IViewObject2), (void**)&m_pViewObject);
		if (FAILED(hr) || NULL == m_pViewObject)	
			break;

        // 创建站点对象
        m_pSite = new IEEmbeddingSite(this);
        m_pSite->AddRef();

        IOleClientSite* pControlSite = static_cast<IOleClientSite*>(m_pSite);
        IAdviseSink*    pAdviseSink = static_cast<IAdviseSink*>(m_pSite);

		// 将站点对象与控件关联起来
		hr = m_pOleObject->SetClientSite(pControlSite);
		if (FAILED(hr))
			break;

		hr = m_pViewObject->SetAdvise(DVASPECT_CONTENT, 0, pAdviseSink);
		if (FAILED(hr))
			break;

		hr = m_pOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, pControlSite, 0, NULL, NULL);

        // 事件监听
        hr = AtlAdvise(m_pWebBrowser,  static_cast<IDispatch*>(m_pSite), DIID_DWebBrowserEvents2, &m_dwEventCookie);
		// 屏蔽脚本错误信息提示
		hr = m_pWebBrowser->put_Silent(VARIANT_TRUE);
		// 禁止拖拽文件
		hr = m_pWebBrowser->put_RegisterAsDropTarget(VARIANT_FALSE);  // [注]: 该调用不要太靠前

		// 获取用于承载IE的窗口
 		CComQIPtr<IOleWindow> pOleWindow = m_pWebBrowser;
 		if (pOleWindow)
        {
 			pOleWindow->GetWindow(&m_hWndShellEmbedding);
        }
        
        if (m_hWndShellEmbedding)
        {
            m_pIIEWrap->Attach(m_hWndShellEmbedding);
        }
	}
	while (0);
}

bool IEWrap::SubclassIEWindow()
{
    UIASSERT(m_hWndIE);

    if (!m_hWndIE)
        return false;

    this->m_thunk.Init( &IEWrap::_WndProc, this );
    WNDPROC pProc = this->m_thunk.GetWNDPROC();
    this->m_oldWndProc = (WNDPROC)(LONG_PTR) ::SetWindowLong( m_hWndIE, GWLP_WNDPROC, (LONG)(LONG_PTR)pProc);

    return true;
}

bool IEWrap::UnSubclassIEWindow()
{
    UIASSERT(m_hWndIE);
    if (!m_hWndIE)
        return false;

    ::SetWindowLong( m_hWndIE, GWLP_WNDPROC, (LONG)(LONG_PTR)m_oldWndProc);
    m_oldWndProc = NULL;
    return true;
}

// 修改为使用HwndHost进行同步，不再使用popup弹出窗口
// void IEWrap::OnSize( UINT nType, int cx, int cy )
// {
// 	if (NULL == m_pWebBrowser)
// 		return;
// 
//     CRect  rc;
//     m_pIIEWrap->GetParentRect(&rc);
// 	SetIEPos(&rc);
// }

HRESULT IEWrap::SetIEPos(LPRECT lprc)
{
	if (NULL == m_pWebBrowser || NULL == lprc)
		return E_INVALIDARG;

	CComQIPtr<IOleInPlaceObject> p = m_pWebBrowser;
	return p->SetObjectRects(lprc, lprc);
}
HRESULT IEWrap::SetIESize(int cx, int cy)
{
	if (NULL == m_pOleObject)
		return E_INVALIDARG;

	SIZEL pxsize = { cx, cy };
	SIZEL hmsize = { 0 };
	AtlPixelToHiMetric(&pxsize, &hmsize);
	return m_pOleObject->SetExtent(DVASPECT_CONTENT, &hmsize);
}

HRESULT IEWrap::InvokeJsFunction(BSTR bstrFuncName)
{
	if (NULL == m_pWebBrowser)
		return E_FAIL;

	CComPtr<IDispatch> pDispDoc = NULL;
	m_pWebBrowser->get_Document(&pDispDoc);
	if (NULL == pDispDoc)
		return E_FAIL;

	CComQIPtr<IHTMLDocument2> pDoc = pDispDoc;
	if (NULL == pDoc)
		return E_FAIL;

	CComPtr<IHTMLWindow2> pWnd = NULL;
	pDoc->get_parentWindow(&pWnd);
	if (NULL == pWnd)
		return E_FAIL;

	CComQIPtr<IDispatch>  pDispatch = pWnd;
	if (NULL == pDispatch)
		return E_FAIL;
	
	DISPID dispid = 0;
	if (FAILED (pDispatch->GetIDsOfNames(IID_NULL,&bstrFuncName,1,LOCALE_SYSTEM_DEFAULT,&dispid))) 
		dispid = -1;

	if(dispid == -1) 
		return E_FAIL;

	DISPPARAMS ps;
	ps.cArgs = 0;
	ps.rgvarg = NULL;
	ps.cNamedArgs = 0;
	ps.rgdispidNamedArgs = NULL;

	CComVariant varResult;
	return pDispatch->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT, DISPATCH_METHOD, &ps, &varResult, NULL, NULL);
}

HRESULT  IEWrap::Navigate(const TCHAR* szUrl)
{
	if (!szUrl)
		return E_INVALIDARG;

	CComVariant v(szUrl);
	return m_pWebBrowser->Navigate2(&v, NULL,NULL,NULL,NULL);
}

LRESULT  IEWrap::_WndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    IEWrap* pThis = (IEWrap*)hwnd;
    return pThis->WndProc( uMsg, wParam, lParam );
}

LRESULT IEWrap::DefWindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if( NULL != m_oldWndProc )
    {
        return ::CallWindowProc( m_oldWndProc, m_hWndIE, uMsg, wParam, lParam );
    }
    return 0;
}

void  IEWrap::OnPaint(IRenderTarget* pRenderTarget, RenderContext* pContext)
{   
    m_bNeedFixAlpha = false;
    if (pContext->m_bRequireAlphaChannel && m_hWndIE)   // 在分层拖拽窗口过程中由于忽略了WS_CLIPCHILDREN样式，导致IE在拖拽中不能显示，造成闪烁，在这里画上去
    {
        if (WINDOW_TRANSPARENT_MODE_LAYERED == UISendMessage(m_pIIEWrap->GetWindowObject(), UI_WM_GET_WINDOW_TRANSPARENT_MODE, 0))
        {
            m_bNeedFixAlpha = true;
            CRect rc;
            ::GetClientRect(m_hWndIE, &rc);

            Render2DC(pRenderTarget->GetBindHDC(), 0, 0, &rc); // hDC已自带偏移
        }
    }
}
// [注]
//   2013.12.24 PrintWindow效率还是太低，因为有可能只刷新一个很小的区域，却还得Print整个窗口
//   在窗口最大化的时候尤为明显，跑马灯非常耗CPU
//   在网上搜索窗口重定向也没有找到实现原理。
//
//  推荐暂时还不要使用分层窗口上的IE，可以使用DWM + ie，只要不将透明区域扩展到IE范围内就行

// PrintWindow和WM_PRINT不同，WM_PRINT是合作式的，需要目标窗口的窗口过程实现对WM_PRINTCLIENT的响应
// PrintWindow却不需要，只需要对方能响应WM_PAINT，它的大致过程如下
// PrintWindow(NtUserPrintWindow)->xxxPrintWindow->(SetRedirectedWindow->CreateRedirectionBitmap)->xxxUpdateWindow->_GetDCEx->NtGdiBitBlt
// xxxPrintWindow首先调用SetRedirectedWindow创建一个重定向位图(CreateRedirectionBitmap)，将该窗口的绘制重定向到这个位图并做初始化、格式转换工作，而不是通常情况下的主帧缓冲(如果是开了桌面组合则是dwm里面的离屏表面)
// 接下来调用xxxUpdateWindow，向目标发送WM_PAINT，此时窗口过程像窗口dc绘制将被重定位到重定向位图，因此dc不会由于被遮挡而被剪裁，不再依赖窗口的位置，内容可以全部保留在重定向位图
// 最后调用_GetDCEx->NtGdiBitBlt将目标窗口的dc的内容bitblt到你提供的dc

// * Sets the window to layered using SetRedirectedWindow()
// * Redraws completely the window using UpdateWindow() in the newly created empty off screen bitmap
// * Gets the DC of the new of the window with GetDCEx()
// * Copies the bitmap to the provided DC using NtGdiBitBlt()
// * Releases the new DC
// * Unsets the layered attribute
// 
// So we now know why PrintWindow() permits to take screenshots of partially or completely hidden windows! 
// This is because of the existence of layered windows!

// prcUpdate相对于IE窗口的客户区域
void  IEWrap::Render2DC(HDC hDCDst, int nxOffset, int nyOffset, CRect* prcUpdate)
{
    HBITMAP hBitmap = m_pIIEWrap->GetUIApplication()->GetCacheBitmap(m_pIIEWrap->GetWidth(), m_pIIEWrap->GetHeight());

    // 为什么不能使用带偏移的DC？绘制出来的全乱了（是内部绘制也使用了偏移吗）
    // 创建一个全新的缓存专门来规避这个问题，然后再Bitblt上去。（这个有点效率低呀）

    HDC hDC = CreateCompatibleDC(NULL);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hDC, hBitmap);

    // 会导致CPU一直运行，内部什么原理？换用PrintWindow
    // 	RECT rcDraw = {0, 0, rcWnd.Width(), rcWnd.Height()};
    // 	HRESULT hr = m_pViewObject->Draw(DVASPECT_CONTENT, -1, 0, 0, NULL, hDC,
    // 		(LPCRECTL)&rcDraw, NULL, NULL, 0);

     // PrintWindow也会导致有些元素在高亮时显示不正常...
     m_bPrintingWindow = true;
     PrintWindow(m_hWndIE, hDC, 0);  // 会再次触发WM_PAINT(wm_ncpaint, wm_erasebkgnd)，加上一个标志
     m_bPrintingWindow = false;

    if (m_bNeedFixAlpha)
    {
        Util::FixAlphaData data = {0};
        data.hBitmap = hBitmap;
        data.bTopDownDib = TRUE;
        data.lprc = prcUpdate;
        data.eMode = Util::SET_ALPHA_255;  // 在这里为什么考虑其它alpha值反而渲染有问题
        Util::FixBitmapAlpha(&data);
    }

    ::BitBlt(
        hDCDst, 
        nxOffset + prcUpdate->left, 
        nyOffset + prcUpdate->top, 
        prcUpdate->Width(), 
        prcUpdate->Height(), 
        hDC, 
        prcUpdate->left, 
        prcUpdate->top, 
        SRCCOPY);

    SelectObject(hDC, hOldBmp);
    DeleteDC(hDC);
}

//
//[注]: IHTMLElementRender::DrawToDC 在IE9中过期了
//
LRESULT	IEWrap::WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    if (WM_PAINT == uMsg && m_bNeedFixAlpha && !m_bPrintingWindow)
    {
		CRect rcUpdate;
		GetUpdateRect(m_hWndIE, &rcUpdate, FALSE);
		if (::IsRectEmpty(&rcUpdate))
		{
			GetClientRect(m_hWndIE, &rcUpdate);
		}

        DefWindowProc(uMsg, wParam, lParam);
        

        CRect  rcWnd;
        m_pIIEWrap->GetWindowRect(&rcWnd);

        HDC hDCDst = m_pIIEWrap->GetRenderLayer()->GetRenderTarget()->GetBindHDC();
        Render2DC(hDCDst, rcWnd.left, rcWnd.top, &rcUpdate);
        OffsetRect(&rcUpdate, rcWnd.left, rcUpdate.top);
        m_pIIEWrap->GetRenderChain()->Commit(&rcUpdate);

        return 0;
    }
    return DefWindowProc(uMsg, wParam, lParam);
}


void IEWrap::DocumentComplete(IDispatch *pDisp, VARIANT *URL)
{
    if (!m_hWndIE)
    {
		if (!m_hWndShellDocObjectView)
		{
			m_hWndShellDocObjectView = GetWindow(m_hWndShellEmbedding, GW_CHILD);
		}

        if (m_hWndShellDocObjectView)
        {
            m_hWndIE = GetWindow(m_hWndShellDocObjectView, GW_CHILD);
        }
        
        // 子类化IE窗口
        if (m_hWndIE)
        {
            SubclassIEWindow();
        }
    }
}

}