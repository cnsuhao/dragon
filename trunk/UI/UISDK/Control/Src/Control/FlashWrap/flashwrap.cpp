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

// ��ʵ�ֵĻ�Flash����ʾ
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
	return E_NOTIMPL;  // ����flashλ��ֱ�ӵ���SetFlashPos����

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

	// ���ڴ�ʱ��û�в��֣���������������
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
    // ʹ�õ�flash�汾���Լ��ٴ���һ��������̣߳����¿ؼ�����ʱ����߳�һֱ����
    // ����ڳ����˳�ʱ��һ��leak�����Ҳ�������������Ȳ����ˣ�
    //
    // PS: qq��C:\Users\libopc\AppData\Roaming\Tencent\QQ\Flash\Dll������һ��flash.ocx
    // ò������汾����ȥ��������̡߳�������ocx������,qq��Ȼ����ϵͳflash
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
		// ����Flash�ؼ�
		if (NULL == m_pOleObject) 
		{
            // ʹ���Դ���flash.ocx
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

		// ����վ�����
		m_pSite = new FlashEmbeddingSite(this);
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
		
		// ��ʼ���ؼ�����
		// ע��ֻ���ڵ�����InitNew֮�����޸�WMode������
		// CComQIPtr<IPersistStreamInit> spPSI = m_pOleObject;
		// hr = spPSI->InitNew();
		// UIASSERT(SUCCEEDED(hr));

 		this->SetWMode(m_eWMode);

		// ע��DoVerb OLEIVERB_INPLACEACTIVATE ��Ҫ����put_Movie���棬����flash�Ქ�ŵĺܿ졣��Ϊʲô������?��
		// ��ʱ��ȥ����IOleInPlaceSiteWindowless�ӿ�
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

	m_pIFlashWrap->SetTabstop(false);   // Label �����н���

	ObjStyle s = {0};
	s.reject_all_mouse_msg = 1;
	m_pIFlashWrap->ModifyObjectStyle(&s, 0);  // Ĭ�ϲ����������Ϣ
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

	// ���������꣬ת���ɿؼ�����
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
    
	// ���ⲿ���ؼ�ˢ��
	m_pIFlashWrap->SetCanRedraw(false);
	if (SUCCEEDED(m_pFlash->LoadMovie(0, (BSTR)m_strFlashUri.c_str())))  // ·�������ڣ�����������᷵��ʧ��
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

// Ĭ��Ϊ����ģʽ:Window��Opaque���޴���ģʽ�����б���ɫ��Transparent���޴���ģʽ�����б���ɫ
// PS: Windowģʽ�ڷֲ㴰�����ǲ�������ˢ�µġ�
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
CallFlashFunction XML ��ʽ���£�

const cstInvoke='<invoke name="%s" returntype="xml"><arguments>%s</arguments></invoke>';
���У����w�ą������£�
	a���ִ���string��ʾ
	b����ֵ��number��ʾ
	c��Boolean��ͱ��^���⣬ֱ�ӂ��f���]�ϵ� <true/> �� <false/>
	d�����������date��ʾ��
	e��δ���x��undefined��ʾ
	f����ֵ�����]�͵� <null/> ��ʾ
�e�����£����f�ִ���Boolean��ͅ���
TsfFlashPlayerAsync.CallFunction(0,'<invoke name="SetFocus2Center" returntype="xml"><arguments><string>�̼����Q</string><true/></arguments></invoke>')
������f�ִ��������п��ܕ�����XML�����P�I�ֻ���Ԫ��������XML����ʧ����
��˽��hʹ��CDATA���C�ִ����FXML�ı����P�I�ֻ���Ԫ��
CDATA��ʽ��<![CDATA[����]]>
���V���ӿ��ԸĞ飺
TsfFlashPlayerAsync.CallFunction(0,'<invoke name="SetFocus2Center" returntype="xml"><arguments><string><![CDATA[�̼����Q]]></string><true/></arguments></invoke>')
*/
HRESULT FlashWrap::CallFlashFunction(LPCTSTR szFuncName, LPCTSTR szArgs, BSTR* pbstrRet)
{
	if (NULL == m_pFlash)
		return E_FAIL;

    long lVertion = 0;
    m_pFlash->FlashVersion(&lVertion);
    int nMajor = HIWORD(lVertion);
    if (nMajor < 9)  // ����������Ҫ��9.0���ϰ汾���õ��ã�����ֵ�Ƕ���û�鵽
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

// swf�ļ��ǲ�����ͣ��
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
			
	X Position X����                       0 Get,Set 
	Y Position Y����                       1 Get,Set 
	X Scale X�������ű���                  2 Get,Set 
	Y Scale Y�������ű���                  3 Get,Set 
	CurrentFrame movie_clip��ǰ����֡��    4 Get 
	TotalFrames movie_clip��֡��           5 Get 
	Alpha movie_clip��͸����               6 Get,Set 
	Visibility movie_clip�Ƿ�ɼ�          7 Get,Set 
	Width movie_clip�Ŀ��                 8 Get 
	Height movie_clip�ĸ߶�                9 Get 
	Rotation movie_clip����ת��           10 Get,Set 
	Target movie_clip��·��               11 Get 
	Framesloaded movie_clip�������֡��   12 Get 
	Name movie_clip��ʵ������             13 Get,Set 
	DropTarget movie_clip����ק           14 Get 
	Url ����movie_clip�Ķ�����url         15 Get 
	HighQuality ���û�������Ϊ������      16 Get,Set 
	FocusRect ��ʾ��ť�߿�                17 Get,Set 
	SoundBufTime Ԥ����������ʱ��         18 Get,Set 
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

