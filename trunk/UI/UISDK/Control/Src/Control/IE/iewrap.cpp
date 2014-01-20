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
            
//            UIASSERT(0 && _T("TODO: δ���"));

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

        // ����վ�����
        m_pSite = new IEEmbeddingSite(this);
        m_pSite->AddRef();

        IOleClientSite* pControlSite = static_cast<IOleClientSite*>(m_pSite);
        IAdviseSink*    pAdviseSink = static_cast<IAdviseSink*>(m_pSite);

		// ��վ�������ؼ���������
		hr = m_pOleObject->SetClientSite(pControlSite);
		if (FAILED(hr))
			break;

		hr = m_pViewObject->SetAdvise(DVASPECT_CONTENT, 0, pAdviseSink);
		if (FAILED(hr))
			break;

		hr = m_pOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, pControlSite, 0, NULL, NULL);

        // �¼�����
        hr = AtlAdvise(m_pWebBrowser,  static_cast<IDispatch*>(m_pSite), DIID_DWebBrowserEvents2, &m_dwEventCookie);
		// ���νű�������Ϣ��ʾ
		hr = m_pWebBrowser->put_Silent(VARIANT_TRUE);
		// ��ֹ��ק�ļ�
		hr = m_pWebBrowser->put_RegisterAsDropTarget(VARIANT_FALSE);  // [ע]: �õ��ò�Ҫ̫��ǰ

		// ��ȡ���ڳ���IE�Ĵ���
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

// �޸�Ϊʹ��HwndHost����ͬ��������ʹ��popup��������
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
    if (pContext->m_bRequireAlphaChannel && m_hWndIE)   // �ڷֲ���ק���ڹ��������ں�����WS_CLIPCHILDREN��ʽ������IE����ק�в�����ʾ�������˸�������ﻭ��ȥ
    {
        if (WINDOW_TRANSPARENT_MODE_LAYERED == UISendMessage(m_pIIEWrap->GetWindowObject(), UI_WM_GET_WINDOW_TRANSPARENT_MODE, 0))
        {
            m_bNeedFixAlpha = true;
            CRect rc;
            ::GetClientRect(m_hWndIE, &rc);

            Render2DC(pRenderTarget->GetBindHDC(), 0, 0, &rc); // hDC���Դ�ƫ��
        }
    }
}
// [ע]
//   2013.12.24 PrintWindowЧ�ʻ���̫�ͣ���Ϊ�п���ֻˢ��һ����С������ȴ����Print��������
//   �ڴ�����󻯵�ʱ����Ϊ���ԣ�����Ʒǳ���CPU
//   ���������������ض���Ҳû���ҵ�ʵ��ԭ��
//
//  �Ƽ���ʱ����Ҫʹ�÷ֲ㴰���ϵ�IE������ʹ��DWM + ie��ֻҪ����͸��������չ��IE��Χ�ھ���

// PrintWindow��WM_PRINT��ͬ��WM_PRINT�Ǻ���ʽ�ģ���ҪĿ�괰�ڵĴ��ڹ���ʵ�ֶ�WM_PRINTCLIENT����Ӧ
// PrintWindowȴ����Ҫ��ֻ��Ҫ�Է�����ӦWM_PAINT�����Ĵ��¹�������
// PrintWindow(NtUserPrintWindow)->xxxPrintWindow->(SetRedirectedWindow->CreateRedirectionBitmap)->xxxUpdateWindow->_GetDCEx->NtGdiBitBlt
// xxxPrintWindow���ȵ���SetRedirectedWindow����һ���ض���λͼ(CreateRedirectionBitmap)�����ô��ڵĻ����ض������λͼ������ʼ������ʽת��������������ͨ������µ���֡����(����ǿ��������������dwm�������������)
// ����������xxxUpdateWindow����Ŀ�귢��WM_PAINT����ʱ���ڹ����񴰿�dc���ƽ����ض�λ���ض���λͼ�����dc�������ڱ��ڵ��������ã������������ڵ�λ�ã����ݿ���ȫ���������ض���λͼ
// ������_GetDCEx->NtGdiBitBlt��Ŀ�괰�ڵ�dc������bitblt�����ṩ��dc

// * Sets the window to layered using SetRedirectedWindow()
// * Redraws completely the window using UpdateWindow() in the newly created empty off screen bitmap
// * Gets the DC of the new of the window with GetDCEx()
// * Copies the bitmap to the provided DC using NtGdiBitBlt()
// * Releases the new DC
// * Unsets the layered attribute
// 
// So we now know why PrintWindow() permits to take screenshots of partially or completely hidden windows! 
// This is because of the existence of layered windows!

// prcUpdate�����IE���ڵĿͻ�����
void  IEWrap::Render2DC(HDC hDCDst, int nxOffset, int nyOffset, CRect* prcUpdate)
{
    HBITMAP hBitmap = m_pIIEWrap->GetUIApplication()->GetCacheBitmap(m_pIIEWrap->GetWidth(), m_pIIEWrap->GetHeight());

    // Ϊʲô����ʹ�ô�ƫ�Ƶ�DC�����Ƴ�����ȫ���ˣ����ڲ�����Ҳʹ����ƫ����
    // ����һ��ȫ�µĻ���ר�������������⣬Ȼ����Bitblt��ȥ��������е�Ч�ʵ�ѽ��

    HDC hDC = CreateCompatibleDC(NULL);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hDC, hBitmap);

    // �ᵼ��CPUһֱ���У��ڲ�ʲôԭ������PrintWindow
    // 	RECT rcDraw = {0, 0, rcWnd.Width(), rcWnd.Height()};
    // 	HRESULT hr = m_pViewObject->Draw(DVASPECT_CONTENT, -1, 0, 0, NULL, hDC,
    // 		(LPCRECTL)&rcDraw, NULL, NULL, 0);

     // PrintWindowҲ�ᵼ����ЩԪ���ڸ���ʱ��ʾ������...
     m_bPrintingWindow = true;
     PrintWindow(m_hWndIE, hDC, 0);  // ���ٴδ���WM_PAINT(wm_ncpaint, wm_erasebkgnd)������һ����־
     m_bPrintingWindow = false;

    if (m_bNeedFixAlpha)
    {
        Util::FixAlphaData data = {0};
        data.hBitmap = hBitmap;
        data.bTopDownDib = TRUE;
        data.lprc = prcUpdate;
        data.eMode = Util::SET_ALPHA_255;  // ������Ϊʲô��������alphaֵ������Ⱦ������
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
//[ע]: IHTMLElementRender::DrawToDC ��IE9�й�����
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
        
        // ���໯IE����
        if (m_hWndIE)
        {
            SubclassIEWindow();
        }
    }
}

}