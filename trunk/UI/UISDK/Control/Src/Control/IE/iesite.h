#pragma once


namespace UI
{
class IEWrap;
interface IIEWrap;


interface IDWebBrowserEvent2
{
    virtual void DocumentComplete(IDispatch *pDisp, VARIANT *URL) = 0;
};

class IEEmbeddingSite : 
	  public IOleClientSite
	, public IOleControlSite
	, public IAdviseSink       // 控件通过该接口调用OnViewChange来刷新自己
	, public IOleInPlaceSite   // , public IOleInPlaceSiteWindowless
// 		, public IOleInPlaceFrame
	, public IDispatch         // 接受ie事件
	, public IDocHostUIHandler // 支持js扩展

//     , public IHTMLPainter
//     , public IElementBehaviorFactory
//     , public IElementBehavior

{
public:
	IEEmbeddingSite(IEWrap* p);
	~IEEmbeddingSite();

#pragma region // IUnknown
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void  **ppvObject);
	virtual ULONG   STDMETHODCALLTYPE AddRef(void);
	virtual ULONG   STDMETHODCALLTYPE Release(void);
#pragma endregion

#pragma region // IDispatch
    virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount( 
        /* [out] */ __RPC__out UINT *pctinfo)
    { return E_NOTIMPL; }

    virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( 
        /* [in] */ UINT iTInfo,
        /* [in] */ LCID lcid,
        /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo)
    {
        return E_NOTIMPL;
    }

    virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( 
        /* [in] */ __RPC__in REFIID riid,
        /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
        /* [range][in] */ UINT cNames,
        /* [in] */ LCID lcid,
        /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId)
    {
        return E_NOTIMPL;
    }

    virtual /* [local] */ HRESULT STDMETHODCALLTYPE Invoke( 
        /* [in] */ DISPID dispIdMember,
        /* [in] */ REFIID riid,
        /* [in] */ LCID lcid,
        /* [in] */ WORD wFlags,
        /* [out][in] */ DISPPARAMS *pDispParams,
        /* [out] */ VARIANT *pVarResult,
        /* [out] */ EXCEPINFO *pExcepInfo,
        /* [out] */ UINT *puArgErr);
#pragma endregion

#pragma region // IAdviseSink
	virtual void STDMETHODCALLTYPE OnDataChange(FORMATETC *pFormatetc, STGMEDIUM *pStgmed)
	{ }
	virtual  void STDMETHODCALLTYPE OnViewChange(DWORD dwAspect, LONG lindex)
	{ }
	virtual void STDMETHODCALLTYPE OnRename(IMoniker *pmk)
	{ }
	virtual void STDMETHODCALLTYPE OnSave(void) 
	{ }
	virtual void STDMETHODCALLTYPE OnClose(void)
	{ }
#pragma endregion

#pragma region // IOleClientSite
	virtual HRESULT STDMETHODCALLTYPE SaveObject(void)
	{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker,  IMoniker **ppmk)
	{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE GetContainer(IOleContainer **ppContainer)
	{ *ppContainer = NULL; return E_NOTIMPL; }  // 直接返回S_OK会导致IE崩溃
	virtual HRESULT STDMETHODCALLTYPE ShowObject(void)
	{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE OnShowWindow(BOOL fShow)
	{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE RequestNewObjectLayout(void)
	{ return S_OK; }
#pragma endregion

#pragma region // IOleControlSite
	virtual HRESULT STDMETHODCALLTYPE OnControlInfoChanged(void)
	{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE LockInPlaceActive(BOOL fLock)
	{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE GetExtendedControl(IDispatch **ppDisp)
	{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE TransformCoords(POINTL *pPtlHimetric, POINTF *pPtfContainer, DWORD dwFlags)
	{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(MSG *pMsg, DWORD grfModifiers)
	{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE OnFocus(BOOL fGotFocus)
	{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE ShowPropertyFrame(void)
	{ return S_OK; }
#pragma endregion

#pragma region // IOleInPlaceSite
	virtual HRESULT STDMETHODCALLTYPE CanInPlaceActivate(void)
	{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivate(void)
	{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE OnUIActivate(void)
	{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
	virtual HRESULT STDMETHODCALLTYPE Scroll(SIZE scrollExtant)
	{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE OnUIDeactivate(BOOL fUndoable)
	{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate(void)
	{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE DiscardUndoState(void)
	{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE DeactivateAndUndo(void)
	{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE OnPosRectChange(LPCRECT lprcPosRect)
	{ return S_OK; }
#pragma endregion

#pragma region // IOleWindow
	virtual HRESULT STDMETHODCALLTYPE GetWindow(HWND *phwnd);
	virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp(BOOL fEnterMode) 
	{ return S_OK; }
#pragma endregion

#pragma region // IDocHostUIHandler
	virtual HRESULT STDMETHODCALLTYPE GetExternal(/* [out] */ IDispatch **ppDispatch);

	virtual HRESULT STDMETHODCALLTYPE ShowContextMenu(/* [in] */ DWORD dwID, /* [in] */ POINT *ppt, /* [in] */ IUnknown *pcmdtReserved, /* [in] */ IDispatch *pdispReserved)
		{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE GetHostInfo(/* [out][in] */ DOCHOSTUIINFO *pInfo);
	virtual HRESULT STDMETHODCALLTYPE ShowUI(/* [in] */ DWORD dwID, /* [in] */ IOleInPlaceActiveObject *pActiveObject, /* [in] */ IOleCommandTarget *pCommandTarget, /* [in] */ IOleInPlaceFrame *pFrame, /* [in] */ IOleInPlaceUIWindow *pDoc)
		{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE HideUI( void)
		{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE UpdateUI( void)
		{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE EnableModeless(/* [in] */ BOOL fEnable)
		{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE OnDocWindowActivate(/* [in] */ BOOL fActivate)
		{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(/* [in] */ BOOL fActivate)
		{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE ResizeBorder(/* [in] */ LPCRECT prcBorder, /* [in] */ IOleInPlaceUIWindow *pUIWindow, /* [in] */ BOOL fRameWindow)
		{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(/* [in] */ LPMSG lpMsg, /* [in] */ const GUID *pguidCmdGroup, /* [in] */ DWORD nCmdID)
		{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE GetOptionKeyPath(/* [annotation][out] */  __out  LPOLESTR *pchKey, /* [in] */ DWORD dw) 
		{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE GetDropTarget(/* [in] */ IDropTarget *pDropTarget, /* [out] */ IDropTarget **ppDropTarget) 
		{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE TranslateUrl(/* [in] */ DWORD dwTranslate, /* [annotation][in] */ __in __nullterminated  OLECHAR *pchURLIn, /* [annotation][out] */ __out  OLECHAR **ppchURLOut) 
		{ return S_OK; }
	virtual HRESULT STDMETHODCALLTYPE FilterDataObject(/* [in] */ IDataObject *pDO, /* [out] */ IDataObject **ppDORet) 
		{ return S_OK; }
#pragma endregion

// #pragma region // IHTMLPainter
//     virtual HRESULT STDMETHODCALLTYPE Draw( 
//         /* [in] */ RECT rcBounds,
//         /* [in] */ RECT rcUpdate,
//         /* [in] */ LONG lDrawFlags,
//         /* [in] */ __RPC__in HDC hdc,
//         /* [in] */ __RPC__in LPVOID pvDrawObject);
// 
//     virtual HRESULT STDMETHODCALLTYPE OnResize( 
//         /* [in] */ SIZE size);
// 
//     virtual HRESULT STDMETHODCALLTYPE GetPainterInfo( 
//         /* [out] */ __RPC__out HTML_PAINTER_INFO *pInfo);
// 
//     virtual HRESULT STDMETHODCALLTYPE HitTestPoint( 
//         /* [in] */ POINT pt,
//         /* [out] */ __RPC__out BOOL *pbHit,
//         /* [out] */ __RPC__out LONG *plPartID);
// 
//     // IElementBehaviorFactory
//     virtual HRESULT STDMETHODCALLTYPE FindBehavior( 
//         /* [in] */ __RPC__in BSTR bstrBehavior,
//         /* [in] */ __RPC__in BSTR bstrBehaviorUrl,
//         /* [in] */ __RPC__in_opt IElementBehaviorSite *pSite,
//         /* [out][retval] */ __RPC__deref_out_opt IElementBehavior **ppBehavior);
// 
//     // MSHTML calls QueryInterface from the IElementBehavior interface to find IHTMLPainter to determine if the behavior is also a rendering behavior.
//     // IElementBehavior
//     virtual HRESULT STDMETHODCALLTYPE Init( 
//         /* [in] */ __RPC__in_opt IElementBehaviorSite *pBehaviorSite);
// 
//     virtual HRESULT STDMETHODCALLTYPE Notify( 
//         /* [in] */ LONG lEvent,
//         /* [out][in] */ __RPC__inout VARIANT *pVar);
// 
//     virtual HRESULT STDMETHODCALLTYPE Detach( void);
// 
// #pragma endregion 


protected:
    void  OnDocumentComplete();

private:
	long       m_lRef;
	IEWrap*    m_pIEWrap;
    IIEWrap*   m_pIIEWrap;
    IWebBrowser2*   m_pWebBrowser;

    long  m_lHostInfoFlag;
    long  m_lBehaviorCookie;
};

}