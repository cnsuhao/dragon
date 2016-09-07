#include "stdafx.h"
#include "flashwrap.h"
#include <atlcomcli.h>
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"

// clsid = {CLSID_Shockwave Flash Object}
// {D27CDB6E-AE6D-11CF-96B8-444553540000}
static const GUID CLSID_Shockwave = 
{ 0xD27CDB6E, 0xAE6D, 0x11CF, { 0x96, 0xB8, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00 } };


//////////////////////////////////////////////////////////////////////////

FlashEmbeddingSite::FlashEmbeddingSite(FlashWrap* p)
{
	m_lRef = 0;
	m_pFlashWrap = p;
    m_pIFlashWrap = p->GetIFlashWrap();

	m_rcInvalidate.SetRectEmpty();
	m_pListener = NULL;
}
FlashEmbeddingSite::~FlashEmbeddingSite()
{
}
HRESULT STDMETHODCALLTYPE FlashEmbeddingSite::QueryInterface(REFIID riid, void **ppvObject)
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
	else if (::IsEqualIID(IID_IOleInPlaceSiteWindowless, riid))
		*ppvObject = static_cast<IOleInPlaceSiteWindowless*>(this);
	else if (::IsEqualIID(IID_IOleInPlaceSiteEx, riid))
		*ppvObject = static_cast<IOleInPlaceSiteEx*>(this);
	else if (::IsEqualIID(IID_IOleInPlaceSite, riid))
		*ppvObject = static_cast<IOleInPlaceSite*>(this);
	else if (::IsEqualIID(IID_IOleWindow, riid))
		*ppvObject = static_cast<IOleWindow*>(this);
	else
		return E_NOINTERFACE;
	
	AddRef();
	return S_OK;
}
ULONG STDMETHODCALLTYPE FlashEmbeddingSite::AddRef(void){return ++m_lRef;}
ULONG STDMETHODCALLTYPE FlashEmbeddingSite::Release(void){--m_lRef; if (0==m_lRef){delete this; return 0;} return m_lRef;}

// 不实现的话Flash不显示
HRESULT STDMETHODCALLTYPE FlashEmbeddingSite::GetWindow(HWND *phwnd)
{
	if (NULL == phwnd || NULL == m_pFlashWrap)
		return E_FAIL;

	*phwnd = m_pIFlashWrap->GetHWND();
	return S_OK;
}


bool FlashEmbeddingSite::GetInvalidateRect(bool bClear, RECT* lprc)
{
	if (::IsRectEmpty(&m_rcInvalidate))
		return false;

	if (lprc)
		::CopyRect(lprc, &m_rcInvalidate);

	if (bClear)
		::SetRectEmpty(&m_rcInvalidate);
	return true;
}

HRESULT STDMETHODCALLTYPE FlashEmbeddingSite::InvalidateRect(LPCRECT pRect, BOOL fErase)
{ 
#if 0
	HWND hWnd = m_pIFlashWrap->GetHWND();
	::InvalidateRect(hWnd, pRect, fErase);
	return 0;
#endif

    if (!m_pIFlashWrap->CanRedraw())
        return S_FALSE;
        
	if (m_pListener)
	{
		m_pListener->OnInvalidateRect(pRect, fErase);
		return S_OK;
	}

	if (NULL == m_pFlashWrap)
		return E_FAIL;

	m_pFlashWrap->GetIFlashWrap()->UpdateObjectEx((RECT*)pRect, 1, true);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE FlashEmbeddingSite::GetWindowContext(IOleInPlaceFrame **ppFrame, IOleInPlaceUIWindow **ppDoc,  LPRECT lprcPosRect, LPRECT lprcClipRect,  LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	return E_NOTIMPL;  // 设置flash位置直接调用SetFlashPos函数

// 	if (ppFrame)
// 		*ppFrame = NULL;
// 	if (ppDoc)
// 		*ppDoc = NULL;
// 
//  	CRect rc;
// 	m_pIFlashWrap->GetRectInLayer(&rc, false);
// 
// 	if (lprcPosRect)
// 	{
// 		CopyRect(lprcPosRect, &rc);
// 	}
// 	if (lprcClipRect)
// 	{
// 		CopyRect(lprcClipRect, &rc);
// 	}
// 	if (lpFrameInfo)
// 	{
// 		lpFrameInfo->fMDIApp = FALSE;
// 		lpFrameInfo->hwndFrame = m_pIFlashWrap->GetHWND();
// 		lpFrameInfo->haccel = NULL;
// 		lpFrameInfo->cAccelEntries = 0;
// 	}
// 	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
FlashWrap::FlashWrap()
{
    m_pIFlashWrap = NULL;

	m_pSite = NULL;
	m_pOleObject = NULL;
	m_pFlash = NULL;
	m_pViewObject = NULL;
	m_pWindowless = NULL;
	m_nFlashWidth = m_nFlashHeight = 0;
	m_eWMode = FLASH_WMODE_OPAQUE;
}
FlashWrap::~FlashWrap()
{
	SAFE_RELEASE(m_pSite);
	SAFE_RELEASE(m_pOleObject); 
	SAFE_RELEASE(m_pFlash);
	SAFE_RELEASE(m_pViewObject);
	SAFE_RELEASE(m_pWindowless);
}
HRESULT FlashWrap::FinalConstruct(IUIApplication* p)
{
	DO_PARENT_PROCESS(IFlashWrap, IControl);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

	// 由于此时还没有布局，不能做其它处理
	HRESULT hr = CoCreateInstance(CLSID_Shockwave, NULL, CLSCTX_INPROC_SERVER, IID_IOleObject, (void**)&m_pOleObject);
	if (FAILED(hr))
	{
		UI_LOG_ERROR(_T("CoCreateInstance Flash Active Control Failed. Ret val=%x"), hr);
		return hr;
	}

	return S_OK;
}

void  FlashWrap::FinalRelease()
{
    SetMsgHandled(FALSE);

    //
    // TODO:
    //
    // Application Verfity: flash ocx Thread handle leak
    // 使用的flash版本会自己再创建一个多余的线程，导致控件销毁时这个线程一直存在
    // 最后在程序退出时报一个leak错误（找不到解决方案，先不管了）
    //
    // PS: qq在C:\Users\libopc\AppData\Roaming\Tencent\QQ\Flash\Dll下面有一份flash.ocx
    // 貌似这个版本不会去创建这个线程。如果这个ocx不存在,qq仍然加载系统flash
    if (m_pOleObject)
    {
        m_pOleObject->Close(OLECLOSE_NOSAVE); 
    }
}

bool FlashWrap::CreateControl()
{
	HRESULT   hr   = S_OK;
	bool      bRet = false;

	do 
	{
		// 创建Flash控件
		if (NULL == m_pOleObject) 
		{
            // 使用自带的flash.ocx
//              HANDLE hDll = LoadLibraryA("flash.ocx");
//              if (hDll!= NULL)          
//              {                  
//                  IClassFactory*  pClassFactory = NULL;                  
//                  GetProcAddress(hDll, "DllGetClassObject");
//                  funcDllGetClassObjectFunc(CLSID_ShockwaveFlash, IID_IClassFactory, (void**)&pClassFactory); 
//                  pClassFactory->CreateInstance(NULL, IID_IOleObject, (void**)&m_pOleObject);        
//                  pClassFactory->Release();                
//              }  

			hr = CoCreateInstance(CLSID_Shockwave, NULL, CLSCTX_INPROC_SERVER, IID_IOleObject, (void**)&m_pOleObject);
		}
		if (FAILED(hr) || NULL == m_pOleObject)
		{
			UI_LOG_ERROR(_T("CoCreateInstance Flash Active Control Failed. Ret val=%x"), hr);
			break;
		}

		hr = m_pOleObject->QueryInterface(__uuidof(IShockwaveFlash), (void**)&m_pFlash);
		if (FAILED(hr) || NULL == m_pFlash)	
			break;

		hr = m_pOleObject->QueryInterface(__uuidof(IViewObjectEx), (void**)&m_pViewObject);
		if (FAILED(hr) || NULL == m_pViewObject)	
			break;

		hr = m_pOleObject->QueryInterface(__uuidof(IOleInPlaceObjectWindowless), (void**)&m_pWindowless);
		if (FAILED(hr) || NULL == m_pWindowless)	
			break;

		// 创建站点对象
		m_pSite = new FlashEmbeddingSite(this);
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
		
		// 初始化控件属性
		// 注：只有在调用了InitNew之后，再修改WMode才有用
		// CComQIPtr<IPersistStreamInit> spPSI = m_pOleObject;
		// hr = spPSI->InitNew();
		// UIASSERT(SUCCEEDED(hr));

 		this->SetWMode(m_eWMode);

		// 注：DoVerb OLEIVERB_INPLACEACTIVATE 不要放在put_Movie后面，否则flash会播放的很快。（为什么会这样?）
		// 此时会去请求IOleInPlaceSiteWindowless接口
		HRESULT hr = m_pOleObject->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, pControlSite, 0, NULL, NULL);
 
		if (!m_strFlashUri.empty())
		{
			hr = m_pFlash->LoadMovie(0, (BSTR)m_strFlashUri.c_str());

			m_nFlashWidth  = FlashPropertyUtil::GetFlashWidth(m_pFlash);
			m_nFlashHeight = FlashPropertyUtil::GetFlashHeight(m_pFlash);
		}

		bRet = true;
	} while (0);

	if (false == bRet)
	{
		SAFE_RELEASE(m_pSite);
		SAFE_RELEASE(m_pOleObject);
		SAFE_RELEASE(m_pFlash);
		SAFE_RELEASE(m_pViewObject);
		SAFE_RELEASE(m_pWindowless);
	}

	return true;
}

void FlashWrap::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = m_nFlashWidth;
    pSize->cy = m_nFlashHeight;
}

void FlashWrap::ResetAttribute()
{
	DO_PARENT_PROCESS(IFlashWrap, IControl);

	SAFE_RELEASE(m_pSite);
	SAFE_RELEASE(m_pOleObject);
	SAFE_RELEASE(m_pFlash);
	SAFE_RELEASE(m_pViewObject);
	SAFE_RELEASE(m_pWindowless);

	m_eWMode = FLASH_WMODE_OPAQUE;
	m_strFlashUri.clear();
	m_nFlashWidth = m_nFlashHeight = 0;

	m_pIFlashWrap->SetTabstop(false);   // Label 不能有焦点

	ObjStyle s = {0};
	s.reject_all_mouse_msg = 1;
	m_pIFlashWrap->ModifyObjectStyle(&s, 0);  // 默认不接收鼠标消息
}

void FlashWrap::SetAttribute(IMapAttribute* pMapAttr, bool bReload)
{
	DO_PARENT_PROCESS(IFlashWrap, IControl);

	LPCTSTR szText = pMapAttr->GetAttr(XML_FLASH_URI, true);
	if (szText)
	{
		if (Util::IsFullPath(szText))
		{
			m_strFlashUri = szText;
		}
		else
		{
			TCHAR szPath[MAX_PATH] = _T("");
			TCHAR szRealPath[MAX_PATH] = _T("");

			Util::GetAppPath_(szPath);
			Util::CalcFullPathByRelative(szPath, szText, szRealPath );
			m_strFlashUri.assign(szRealPath);
		}
	}

	szText = pMapAttr->GetAttr(XML_FLASH_WMODE, true);
	if (szText)
	{
		if (0 == _tcscmp(XML_FLASH_WMODE_WINDOW, szText))
			m_eWMode = FLASH_WMODE_WINDOW;
		else if (0 == _tcscmp(XML_FLASH_WMODE_OPAQUE, szText))
			m_eWMode = FLASH_WMODE_OPAQUE;
		else if (0 == _tcscmp(XML_FLASH_WMODE_TRANSPARENT, szText))
			m_eWMode = FLASH_WMODE_TRANSPARENT;
	}

	if (NULL == m_pSite)
		CreateControl();
}

LRESULT  FlashWrap::OnMouseMsg(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	if (!m_pWindowless)
	{
		SetMsgHandled(FALSE);
		return 0;
	}

	// 将窗口坐标，转换成控件坐标
	POINT  pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
	m_pIFlashWrap->WindowPoint2ObjectPoint(&pt, &pt, true);

	LRESULT  lr = 0;
	m_pWindowless->OnWindowMessage(nMsg, wParam, MAKELPARAM(pt.x, pt.y), &lr);

	return lr;
}

void FlashWrap::OnPaint(IRenderTarget* pRenderTarget)
{
	if (NULL == m_pFlash)
		return;

	HDC hDC = pRenderTarget->GetHDC();
	DrawFlash(hDC);

	m_pSite->GetInvalidateRect(true, NULL);
}
void FlashWrap::DrawFlash(HDC hDC)
{
	if (NULL == m_pViewObject)
		return;

	CRect rc(0, 0, m_pIFlashWrap->GetWidth(), m_pIFlashWrap->GetHeight());
//	m_pIFlashWrap->GetWindowRect(&rc);

	HRESULT hr = m_pViewObject->Draw(
				DVASPECT_CONTENT, -1, NULL, NULL, NULL, hDC,
				reinterpret_cast<LPCRECTL>(&rc), NULL, NULL, 0);
}
void FlashWrap::OnSize( UINT nType, int cx, int cy )
{
	if (NULL == m_pFlash)
		return;

    SetMsgHandled(FALSE);

//     CRect rc;
//     m_pIFlashWrap->GetWindowRect(&rc);
	CRect rc (0, 0, cx, cy);
	SetFlashPos(&rc);
}

void FlashWrap::SetFlashUri(LPCTSTR szUri)
{ 
    if (!m_pFlash)
        return;

    m_pFlash->StopPlay();
    m_strFlashUri.clear();
    m_nFlashHeight = 0;
    m_nFlashWidth = 0;

	if (!szUri)
        return;

	String strUri = szUri;
	if (Util::IsFullPath(strUri.c_str()))
	{
		m_strFlashUri = strUri;
	}
	else
	{
		TCHAR szPath[MAX_PATH] = _T("");
		TCHAR szRealPath[MAX_PATH] = _T("");

		Util::GetAppPath_(szPath);
		Util::CalcFullPathByRelative(szPath, strUri.c_str(), szRealPath );
		m_strFlashUri.assign(szRealPath);
	}

    if (!PathFileExists(m_strFlashUri.c_str()))
        return;
    
	// 由外部来控件刷新
	m_pIFlashWrap->SetCanRedraw(false);
	if (SUCCEEDED(m_pFlash->LoadMovie(0, (BSTR)m_strFlashUri.c_str())))  // 路径不存在，这个函数不会返回失败
	{
		m_nFlashWidth  = FlashPropertyUtil::GetFlashWidth(m_pFlash);
		m_nFlashHeight = FlashPropertyUtil::GetFlashHeight(m_pFlash);
	}
	m_pIFlashWrap->SetCanRedraw(true);

    if (m_nFlashHeight == 0 || m_nFlashWidth == 0) 
    {
        m_pFlash->StopPlay();
        m_strFlashUri.clear();
        m_nFlashHeight = 0;
        m_nFlashWidth = 0;
    }
}

bool FlashWrap::IsAvailable()
{
	if (!m_pFlash)
		return false;

	return true;
}

HRESULT FlashWrap::SetFlashPos(LPRECT lprc)
{
	if (NULL == m_pFlash || NULL == lprc)
		return E_INVALIDARG;

	CComQIPtr<IOleInPlaceObject> p = m_pFlash;
	return p->SetObjectRects(lprc, lprc);
}
HRESULT FlashWrap::SetFlashSize(int cx, int cy)
{
	if (NULL == m_pOleObject)
		return E_INVALIDARG;

	SIZEL pxsize = { cx, cy };
	SIZEL hmsize = { 0 };
	AtlPixelToHiMetric(&pxsize, &hmsize);
	return m_pOleObject->SetExtent(DVASPECT_CONTENT, &hmsize);
}

// 默认为窗口模式:Window。Opaque是无窗口模式，但有背景色，Transparent是无窗口模式，但有背景色
// PS: Window模式在分层窗口上是不能正常刷新的。
void FlashWrap::SetWMode(FLASH_WMODE e)
{
	m_eWMode = e;
	if (NULL == m_pFlash)
		return;

	switch (m_eWMode)
	{
	case FLASH_WMODE_OPAQUE:
		m_pFlash->put_WMode(L"Opaque");
		break;

	case FLASH_WMODE_TRANSPARENT:
		m_pFlash->put_WMode(L"Transparent");
		break;

	case FLASH_WMODE_WINDOW:
		m_pFlash->put_WMode(L"Window");
		break;
	}	
}

void FlashWrap::SetFlashInvalidateListener(IFlashInvalidateListener* p)
{
	if (m_pSite)
	{
		m_pSite->SetFlashInvalidateListener(p);
	}
}
/*
CallFlashFunction XML 格式如下：

const cstInvoke='<invoke name="%s" returntype="xml"><arguments>%s</arguments></invoke>';
其中，具體的參數如下：
	a、字串以string表示
	b、數值以number表示
	c、Boolean類型比較特殊，直接傳遞自閉合的 <true/> 或 <false/>
	d、日期類型以date表示、
	e、未定義以undefined表示
	f、空值以自閉和的 <null/> 表示
舉例如下：傳遞字串和Boolean類型參數
TsfFlashPlayerAsync.CallFunction(0,'<invoke name="SetFocus2Center" returntype="xml"><arguments><string>商家名稱</string><true/></arguments></invoke>')
如果傳遞字串參數，有可能會遇到XML保留關鍵字或字元，會導致XML解析失敗。
因此建議使用CDATA來保證字串出現XML的保留關鍵字或字元。
CDATA格式：<![CDATA[內容]]>
上訴例子可以改為：
TsfFlashPlayerAsync.CallFunction(0,'<invoke name="SetFocus2Center" returntype="xml"><arguments><string><![CDATA[商家名稱]]></string><true/></arguments></invoke>')
*/
HRESULT FlashWrap::CallFlashFunction(LPCTSTR szFuncName, LPCTSTR szArgs, BSTR* pbstrRet)
{
	if (NULL == m_pFlash)
		return E_FAIL;

    long lVertion = 0;
    m_pFlash->FlashVersion(&lVertion);
    int nMajor = HIWORD(lVertion);
    if (nMajor < 9)  // 在这里至少要求9.0以上版本才让调用，具体值是多少没查到
        return E_FAIL;

	String  str = _T("<invoke name='");
	str.append(szFuncName);
	str.append(_T("' returntype='xml'><arguments>"));
	str.append(szArgs);
	str.append(_T("</arguments></invoke>"));

	if (NULL == pbstrRet)
	{
		CComBSTR bstr;
		return m_pFlash->CallFunction((BSTR)str.c_str(), &bstr);
	}
	return m_pFlash->CallFunction((BSTR)str.c_str(), pbstrRet);
}

// swf文件是不能暂停的
void  FlashWrap::Pause()
{
    if (m_pFlash)
        m_pFlash->StopPlay();
}
void  FlashWrap::Play()
{
    if (m_pFlash)
        m_pFlash->Play();
}
//////////////////////////////////////////////////////////////////////////


#if 0
	TGetProperty(nameOfTargetMovieClip, propertyIndex) 
			
	X Position X坐标                       0 Get,Set 
	Y Position Y坐标                       1 Get,Set 
	X Scale X方向缩放比率                  2 Get,Set 
	Y Scale Y方向缩放比率                  3 Get,Set 
	CurrentFrame movie_clip当前所在帧数    4 Get 
	TotalFrames movie_clip总帧数           5 Get 
	Alpha movie_clip的透明度               6 Get,Set 
	Visibility movie_clip是否可见          7 Get,Set 
	Width movie_clip的宽度                 8 Get 
	Height movie_clip的高度                9 Get 
	Rotation movie_clip的旋转度           10 Get,Set 
	Target movie_clip的路径               11 Get 
	Framesloaded movie_clip已载入的帧数   12 Get 
	Name movie_clip的实体名字             13 Get,Set 
	DropTarget movie_clip的拖拽           14 Get 
	Url 包含movie_clip的动画的url         15 Get 
	HighQuality 设置画面质量为高质量      16 Get,Set 
	FocusRect 显示按钮边框                17 Get,Set 
	SoundBufTime 预设声音缓冲时间         18 Get,Set 
#endif
int FlashPropertyUtil::GetFlashWidth(IShockwaveFlash* pFlash)
{
	if (NULL == pFlash)
		return 0;

	CComBSTR bstrWidth;
	HRESULT hr = pFlash->TGetProperty(L"_level0", 8, &bstrWidth);
    if (FAILED(hr))
        return 0;

	return _wtoi(bstrWidth);
}
int FlashPropertyUtil::GetFlashHeight(IShockwaveFlash* pFlash)
{
	if (NULL == pFlash)
		return 0;

	CComBSTR bstrHeight;
	HRESULT hr = pFlash->TGetProperty(L"_level0", 9, &bstrHeight);
    if (FAILED(hr))
        return 0;

	return _wtoi(bstrHeight);
}

