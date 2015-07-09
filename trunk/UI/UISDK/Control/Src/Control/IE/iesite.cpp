#include "stdafx.h"
#include "iesite.h"
#include "UISDK\Control\Inc\Interface\iiewrap.h"
#include "iewrap.h"
#include "exdispid.h"

namespace UI
{

IEEmbeddingSite::IEEmbeddingSite(IEWrap* p)
{
    UIASSERT(p);

	m_lRef = 0;
	m_pIEWrap = p;
    m_pIIEWrap = p->GetIIEWrap();
    m_pWebBrowser = p->GetWebBrowser2();

    m_lBehaviorCookie = 0;
    m_lHostInfoFlag = DOCHOSTUIFLAG_NO3DBORDER		// 无边框
        /*|DOCHOSTUIFLAG_USE_WINDOWLESS_SELECTCONTROL*/;  // 这个标志是使用无窗口的<selectt>(combobox)的意思
                    /*DOCHOSTUIFLAG_SCROLL_NO |*/       // 不显示滚动条
    /*
    如果要根据窗口大小动态显示滚动条，则可以重写OnNavigateComplete函数，增加如下代码：

    CComPtr<IHTMLElement> spElement;
    spDocument->get_body(&spElement);
    if (NULL == spElement)
    return;

    CComPtr<IHTMLBodyElement> spBodyElement;
    spElement->QueryInterface(__uuidof(IHTMLBodyElement), (void**)&spBodyElement);
    if (NULL == spBodyElement)
    return;

    spBodyElement->put_scroll(_bstr_t(_T("auto")));

    */
}
IEEmbeddingSite::~IEEmbeddingSite()
{
}
HRESULT STDMETHODCALLTYPE IEEmbeddingSite::QueryInterface(REFIID riid, void **ppvObject)
{
	if (NULL == ppvObject)
		return E_INVALIDARG;

	if (::IsEqualIID(IID_IUnknown, riid))
		*ppvObject = static_cast<IUnknown*>((IOleClientSite*)this);
	else if (::IsEqualIID(IID_IOleClientSite, riid))
		*ppvObject = static_cast<IOleClientSite*>(this);
	else if (::IsEqualIID(IID_IOleControlSite, riid))
		*ppvObject = static_cast<IOleControlSite*>(this);
	else if (::IsEqualIID(IID_IAdviseSink, riid))
		*ppvObject = static_cast<IAdviseSink*>(this);
	else if (::IsEqualIID(IID_IOleInPlaceSite, riid))
		*ppvObject = static_cast<IOleInPlaceSite*>(this);
	else if (::IsEqualIID(IID_IOleWindow, riid))
		*ppvObject = static_cast<IOleWindow*>(this);
	else if (::IsEqualIID(IID_IDocHostUIHandler, riid))
		*ppvObject = static_cast<IDocHostUIHandler*>(this);
    else if (::IsEqualIID(IID_IDispatch, riid))
        *ppvObject = static_cast<IDispatch*>(this);
//     else if (::IsEqualIID(IID_IHTMLPainter, riid))
//         *ppvObject = static_cast<IHTMLPainter*>(this);
//     else if (::IsEqualIID(IID_IElementBehaviorFactory, riid))
//         *ppvObject = static_cast<IElementBehaviorFactory*>(this);
//     else if (::IsEqualIID(IID_IElementBehavior, riid))
//         *ppvObject = static_cast<IElementBehavior*>(this);
	else
		return E_NOINTERFACE;

	AddRef();
	return S_OK;
}
ULONG STDMETHODCALLTYPE IEEmbeddingSite::AddRef(void)
{
	return ++m_lRef;
}
ULONG STDMETHODCALLTYPE IEEmbeddingSite::Release(void)
{
	--m_lRef; 
	if (0==m_lRef)
	{
		delete this; 
		return 0;
	} 
	return m_lRef;
}

// 设置自己的父窗口
HRESULT STDMETHODCALLTYPE IEEmbeddingSite::GetWindow(HWND *phwnd)
{
	if (NULL == phwnd || NULL == m_pIEWrap)
		return E_FAIL;

	*phwnd = m_pIIEWrap->GetHWND();

// 	if (NULL == m_pIEWrap->GetPopupIEWindow())
// 		*phwnd = m_pIIEWrap->GetHWND();
// 	else
// 		*phwnd = m_pIEWrap->GetPopupIEWindow()->GetHWND();

	return S_OK;
}

// 默认IE控件会显示一个3D Border，将其去掉
HRESULT STDMETHODCALLTYPE IEEmbeddingSite::GetHostInfo(/* [out][in] */ DOCHOSTUIINFO *pInfo)
{
    pInfo->cbSize  = sizeof(DOCHOSTUIINFO);
    pInfo->dwFlags = m_lHostInfoFlag;
    pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
    return S_OK;  

}

HRESULT STDMETHODCALLTYPE IEEmbeddingSite::GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc,  LPRECT lprcPosRect, LPRECT lprcClipRect,  LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
    return E_NOTIMPL;
	if (ppFrame)
		*ppFrame = NULL;
	if (ppDoc)
		*ppDoc = NULL;

	CRect rc;
	m_pIIEWrap->GetParentRect(&rc);

	if (lprcPosRect)
	{
		CopyRect(lprcPosRect, &rc);
	}
	if (lprcClipRect)
	{
		CopyRect(lprcClipRect, &rc);
	}
	if (lpFrameInfo)
	{
		lpFrameInfo->cb = sizeof(OLEINPLACEFRAMEINFO);
		lpFrameInfo->fMDIApp = FALSE;
		GetWindow(&lpFrameInfo->hwndFrame);
		lpFrameInfo->haccel = NULL;
		lpFrameInfo->cAccelEntries = 0;
	}
	return S_OK;
}

#pragma region
void CppCall()
{
	MessageBox(NULL, L"您调用了CppCall", L"提示(C++)", 0);
}
class ClientCall:public IDispatch
{
	long _refNum;
public:
	ClientCall()
	{
		_refNum = 1;
	}
	~ClientCall(void)
	{
	}
public:

	// IUnknown Methods

	STDMETHODIMP QueryInterface(REFIID iid,void**ppvObject)
	{
		*ppvObject = NULL;
		if (iid == IID_IUnknown)    *ppvObject = this;
		else if (iid == IID_IDispatch)    *ppvObject = (IDispatch*)this;
		if(*ppvObject)
		{
			AddRef();
			return S_OK;
		}
		return E_NOINTERFACE;
	}

	STDMETHODIMP_(ULONG) AddRef()
	{
		return ::InterlockedIncrement(&_refNum);
	}

	STDMETHODIMP_(ULONG) Release()
	{
		::InterlockedDecrement(&_refNum);
// 		if(_refNum == 0)
// 		{
// 			delete this;
// 		}
		return _refNum;
	}

	// IDispatch Methods

	HRESULT _stdcall GetTypeInfoCount(
		unsigned int * pctinfo) 
	{
		return E_NOTIMPL;
	}

	HRESULT _stdcall GetTypeInfo(
		unsigned int iTInfo,
		LCID lcid,
		ITypeInfo FAR* FAR* ppTInfo) 
	{
		return E_NOTIMPL;
	}

	HRESULT _stdcall GetIDsOfNames(
		REFIID riid, 
		OLECHAR FAR* FAR* rgszNames, 
		unsigned int cNames, 
		LCID lcid, 
		DISPID FAR* rgDispId 
		)
	{
		if(lstrcmp(rgszNames[0], L"CppCall")==0)
		{
			//网页调用window.external.CppCall时，会调用这个方法获取CppCall的ID
			*rgDispId = 100;
		}
		return S_OK;
	}

	HRESULT _stdcall Invoke(
		DISPID dispIdMember,
		REFIID riid,
		LCID lcid,
		WORD wFlags,
		DISPPARAMS* pDispParams,
		VARIANT* pVarResult,
		EXCEPINFO* pExcepInfo,
		unsigned int* puArgErr
		)
	{
		if(dispIdMember == 100)
		{
			//网页调用CppCall时，或根据获取到的ID调用Invoke方法
			CppCall();
		}
		return S_OK;
	}
};
#pragma endregion




HRESULT STDMETHODCALLTYPE IEEmbeddingSite::GetExternal(/* [out] */ IDispatch **ppDispatch)
{
	static ClientCall c;
	*ppDispatch = static_cast<IDispatch*>(&c);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE IEEmbeddingSite::Invoke( 
    /* [in] */ DISPID dispIdMember,
    /* [in] */ REFIID riid,
    /* [in] */ LCID lcid,
    /* [in] */ WORD wFlags,
    /* [out][in] */ DISPPARAMS *pDispParams,
    /* [out] */ VARIANT *pVarResult,
    /* [out] */ EXCEPINFO *pExcepInfo,
    /* [out] */ UINT *puArgErr)
{
    switch (dispIdMember)
    {
    case DISPID_DOCUMENTCOMPLETE:
        {
            if (m_pIEWrap)
            {
                this->OnDocumentComplete();
                m_pIEWrap->DocumentComplete((pDispParams->rgvarg[1]).pdispVal, &(pDispParams->rgvarg[0]));
                return S_OK;
            }
        }
        break;
    }
   
    return E_NOTIMPL;
}

// http://www.codeproject.com/Articles/6845/Implementing-snap-to-grid-in-an-MSHTML-based-appli
// http://msdn.microsoft.com/en-us/library/aa753628(v=vs.85).aspx
void  IEEmbeddingSite::OnDocumentComplete()
{
}
//     HRESULT  hr = S_OK;
// 
//     CComPtr<IHTMLDocument2>  pDoc = NULL;
//     CComPtr<IDispatch>       pDisp = NULL;
//     CComPtr<IHTMLElement>    pBody = NULL;
//     CComPtr<IHTMLElement2>   pBody2 = NULL;
// 
//     if (FAILED(m_pWebBrowser->get_Document(&pDisp)) || NULL == pDisp)
//         return;
// 
//     hr = pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pDoc);
//     if (!pDoc)
//         return;
// 
//     hr = pDoc->get_body(&pBody);
//     if (!pBody)
//         return;
//     hr = pBody->QueryInterface(IID_IHTMLElement2, (void**)&pBody2);
//     if (!pBody2)
//         return;
// 
//     if (m_lBehaviorCookie)
//     {
//         VARIANT_BOOL dummy;
//         hr = pBody2->removeBehavior(m_lBehaviorCookie, &dummy);
//         m_lBehaviorCookie = 0;
//     }
// 
//     VARIANT  vFactory;
//     vFactory.vt = VT_UNKNOWN;
//     vFactory.punkVal = static_cast<IUnknown*>(
//         static_cast<IElementBehaviorFactory*>(this));
// 
//     //TODO: punkVal需要addref 吗
// 
//     hr = pBody2->addBehavior(NULL, &vFactory, &m_lBehaviorCookie);
// 
// }
// 
// HRESULT STDMETHODCALLTYPE IEEmbeddingSite::FindBehavior( 
//     /* [in] */ __RPC__in BSTR bstrBehavior,
//     /* [in] */ __RPC__in BSTR bstrBehaviorUrl,
//     /* [in] */ __RPC__in_opt IElementBehaviorSite *pSite,
//     /* [out][retval] */ __RPC__deref_out_opt IElementBehavior **ppBehavior)
// {
//     *ppBehavior = static_cast<IElementBehavior*>(this);
//     this->AddRef();   // 必须Add一次
// 
//     return S_OK;
// }
// 
// // IElementBehavior
// HRESULT STDMETHODCALLTYPE IEEmbeddingSite::Init( /* [in] */ __RPC__in_opt IElementBehaviorSite *pBehaviorSite)
// {
//     CComPtr<IHTMLPaintSite>  pPaintSite = NULL;
//     HRESULT	hr = pBehaviorSite->QueryInterface(IID_IHTMLPaintSite, (void **) &pPaintSite);
// 
//     if (pPaintSite)
//         pPaintSite->InvalidateRect(NULL);
// 
//     return hr;
// }
// 
// HRESULT STDMETHODCALLTYPE IEEmbeddingSite::Notify( 
//     /* [in] */ LONG lEvent,
//     /* [out][in] */ __RPC__inout VARIANT *pVar)
// {
//     if (pVar == NULL)
//         return E_POINTER;
// 
//     return E_NOTIMPL;
// }
// 
// HRESULT STDMETHODCALLTYPE IEEmbeddingSite::Detach( void)
// {
//     CComPtr<IHTMLDocument2>  pDoc = NULL;
//     CComPtr<IDispatch>       pDisp = NULL;
//     CComPtr<IHTMLElement>    pBody = NULL;
//     CComPtr<IHTMLElement2>   pBody2 = NULL;
// 
//     if (FAILED(m_pWebBrowser->get_Document(&pDisp)) || NULL == pDisp)
//         return E_FAIL;
// 
//     HRESULT hr = pDisp->QueryInterface(IID_IHTMLDocument2, (void**)&pDoc);
//     if (!pDoc)
//         return E_FAIL;
// 
//     hr = pDoc->get_body(&pBody);
//     if (!pBody)
//         return E_FAIL;
//     hr = pBody->QueryInterface(IID_IHTMLElement2, (void**)&pBody2);
//     if (!pBody2)
//         return E_FAIL;
// 
//     if (m_lBehaviorCookie)
//     {
//         VARIANT_BOOL dummy;
//         hr = pBody2->removeBehavior(m_lBehaviorCookie, &dummy);
//         m_lBehaviorCookie = 0;
//     }
//     return S_OK;
// }
// HRESULT STDMETHODCALLTYPE IEEmbeddingSite::Draw( 
//     /* [in] */ RECT rcBounds,
//     /* [in] */ RECT rcUpdate,
//     /* [in] */ LONG lDrawFlags,
//     /* [in] */ __RPC__in HDC hdc,
//     /* [in] */ __RPC__in LPVOID pvDrawObject)
// {
// 
//     Gdiplus::SolidBrush  brush(Gdiplus::Color::Yellow);
//     Gdiplus::Graphics g(hdc);
//     g.FillRectangle(&brush, rcUpdate.left, rcUpdate.top, rcUpdate.right-rcUpdate.left, rcUpdate.bottom-rcUpdate.top);
//     //FillRect(hdc, &rcUpdate, (HBRUSH)GetStockObject(BLACK_BRUSH));
//     return S_OK;
// }
// 
// HRESULT STDMETHODCALLTYPE IEEmbeddingSite::OnResize( 
//     /* [in] */ SIZE size)
// {
//     return S_OK;
// }
// 
// HRESULT STDMETHODCALLTYPE  IEEmbeddingSite::GetPainterInfo( 
//     /* [out] */ __RPC__out HTML_PAINTER_INFO *pInfo)
// {
//     if (pInfo == NULL)
//         return E_POINTER;
// 
//     pInfo->lFlags = HTMLPAINTER_TRANSPARENT;
//     pInfo->lZOrder = HTMLPAINT_ZORDER_BELOW_CONTENT;
// 
//     memset(&pInfo->iidDrawObject, 0, sizeof(IID));
// 
//     pInfo->rcExpand.left = 0;
//     pInfo->rcExpand.right = 0;
//     pInfo->rcExpand.top = 0;
//     pInfo->rcExpand.bottom = 0;
// 
//     return S_OK;
// }
// 
// HRESULT STDMETHODCALLTYPE IEEmbeddingSite::HitTestPoint( 
//     /* [in] */ POINT pt,
//     /* [out] */ __RPC__out BOOL *pbHit,
//     /* [out] */ __RPC__out LONG *plPartID)
// {
//     if (pbHit == NULL)
//         return E_POINTER;
// 
//     if (plPartID == NULL)
//         return E_POINTER;
// 
//     return E_NOTIMPL;
// }

}
