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

		// 屏蔽脚本错误信息提示
		m_pWebBrowser->put_Silent(VARIANT_TRUE);

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

		HRESULT hr = m_pOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, pControlSite, 0, NULL, NULL);

        // 事件监听
        hr = AtlAdvise(m_pWebBrowser,  static_cast<IDispatch*>(m_pSite), DIID_DWebBrowserEvents2, &m_dwEventCookie);

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

void IEWrap::OnSize( UINT nType, int cx, int cy )
{
	if (NULL == m_pWebBrowser)
		return;

    CRect  rc;
    m_pIIEWrap->GetParentRect(&rc);
	SetIEPos(&rc);
}

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

LRESULT	IEWrap::WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    static bool isPrintingWindow = false;

    if (WM_PAINT == uMsg && m_pViewObject && !isPrintingWindow)
    {
        DefWindowProc(uMsg, wParam, lParam);
#if 0
         CRect  rcWnd;
         m_pIIEWrap->GetWindowRect(&rcWnd);
 
         m_pIIEWrap->GetUIApplication()->GetCacheDC()
 		CRect rcDraw(rcWnd);
 		rcDraw.right = rcWnd.Width();
 		rcDraw.bottom = rcWnd.Height();
 		rcDraw.left = rcDraw.top = 0;
         
 		HDC hDC = m_pIIEWrap->GetRenderLayer()->GetRenderTarget()->GetBindHDC();
  		SetViewportOrgEx(hDC, rcWnd.left, rcWnd.top,NULL);
//  		HRGN hRgn = CreateRectRgnIndirect(&rcWnd);
//  		SelectClipRgn(hDC, hRgn);
//  		DeleteObject(hRgn);
 
//  		HRESULT hr = m_pViewObject->Draw(DVASPECT_CONTENT, -1, 0, 0, NULL, hDC,
//  			(LPCRECTL)&rcWnd, NULL, NULL, 0);

        isPrintingWindow = true;
        PrintWindow(m_hWndIE, hDC, PW_CLIENTONLY);
        isPrintingWindow = false;
 		SetViewportOrgEx(hDC, 0,0,NULL);
 		SelectClipRgn(hDC, NULL);
        Util::FixGdiAlpha(hDC, &rcWnd);
 
 		m_pIIEWrap->GetRenderChain()->Commit(&rcWnd);
#elif 1
    CRect  rcWnd;
    m_pIIEWrap->GetWindowRect(&rcWnd);

	CRect rcDraw;
	rcDraw.right = rcWnd.Width();
	rcDraw.bottom = rcWnd.Height();
	rcDraw.left = rcDraw.top = 0;

	HBITMAP hBitmap = m_pIIEWrap->GetUIApplication()->GetCacheBitmap(rcWnd.Width(), rcWnd.Height());

    // 为什么不能使用带偏移的DC？绘制出来的全乱了
	HDC hDC = CreateCompatibleDC(NULL);
	HBITMAP hOldBmp = (HBITMAP)SelectObject(hDC, hBitmap);

// 	HRESULT hr = m_pViewObject->Draw(DVASPECT_CONTENT, -1, 0, 0, NULL, hDC,
// 		(LPCRECTL)&rcDraw, NULL, NULL, 0);

    isPrintingWindow = true;
    PrintWindow(m_hWndIE, hDC, 0);
    isPrintingWindow = false;
	Util::FixGdiAlpha(hDC, &rcDraw);

    HDC hDCDst = m_pIIEWrap->GetRenderLayer()->GetRenderTarget()->GetBindHDC();
    ::BitBlt(hDCDst, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), hDC, 0, 0, SRCCOPY);
    m_pIIEWrap->GetRenderChain()->Commit(&rcWnd);

    SelectObject(hDC, hOldBmp);
	DeleteDC(hDC);
#endif

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