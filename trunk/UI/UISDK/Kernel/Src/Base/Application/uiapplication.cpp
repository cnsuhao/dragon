#include "stdafx.h"
#include "uiapplication.h"

#include "UISDK\Kernel\Inc\Interface\iuires.h"
#include "UISDK\Kernel\Src\Atl\image.h"
#include "UISDK\Kernel\Src\UIObject\Control\control.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\UIObject\Window\window.h"
#include "UISDK\Kernel\Src\UIObject\Window\customwindow.h"
#include "UISDK\Kernel\Src\Renderbase\colorrender\colorrender.h"
#include "UISDK\Kernel\Src\Renderbase\textrenderbase\textrender.h"
#include "UISDK\Kernel\Src\Renderbase\imagerender\imagerender.h"
#include "UISDK\Kernel\Src\Helper\layout\layout.h"
#include "UISDK\Kernel\Src\Helper\layout\stacklayout.h"
#include "UISDK\Kernel\Src\Helper\layout\canvaslayout.h"
#include "UISDK\Kernel\Src\Helper\layout\cardlayout.h"
#include "UISDK\Kernel\Src\Helper\layout\gridlayout.h"
#include "UISDK\Kernel\Src\Helper\layout\averagelayout.h"
#include "UISDK\Kernel\Src\UIObject\HwndHost\HwndHost.h"
#include "UISDK\Kernel\Src\UIObject\Panel\ScrollPanel\scrollpanel.h"
#include "UISDK\Kernel\Src\Resource\skinres.h"
#include "UISDK\Kernel\Src\Resource\imagemanager.h"
#include "UISDK\Kernel\Src\Resource\colormanager.h"
#include "UISDK\Kernel\Src\Resource\fontmanager.h"
#include "UISDK\Kernel\Src\Resource\stylemanager.h"
#include "UISDK\Kernel\Src\Resource\layoutmanager.h"
#include "UISDK\Kernel\Inc\Interface\iuieditor.h"
//#include "UISDK\Project\UI3D\inc\inc.h"
#include "UISDK\Kernel\Src\Util\Gif\giftimermanager.h"
#include "UISDK\Project\UICompositor\Inc\ihardwarecompositor.h"


ILog*     UIApplication::s_pLog = NULL;
long      UIApplication::s_lUiLogCookie = 0;
long      UIApplication::s_lAppCount = 0;
HMODULE   UIApplication::s_hUID2D = NULL;
HMODULE   UIApplication::s_hUID3D = NULL;


UIApplication::UIApplication(IUIApplication* p) : 
    m_pUIApplication(p),
    m_WndForwardPostMsg(this), 
    m_TopWindowMgr(this)
{
    s_lAppCount++;
}

void  UIApplication::x_Init()
{
    //	::CoInitialize(0);
    HRESULT  hr = OleInitialize(0);  // 需要注册richedit的drag drop，因此用ole初始化
	(hr);

    m_bDesignMode = false;
    m_pUIEditor = NULL;

    m_skinMgr.SetUIApplication(m_pUIApplication);

    m_pAnimateMgr = NULL;
    m_pAnimateMgr = new AnimateManager;
    m_pAnimateMgr->SetUIApplication(this);

	m_pGifTimerMgr = NULL;
	m_pGifTimerMgr = new GifTimerManager();
	m_pGifTimerMgr->Init(this);

    m_ToolTipMgr.Init(this);

    // 获取操作系统版本信息
    ZeroMemory(&m_osvi, sizeof(OSVERSIONINFOEX));
    m_osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    GetVersionEx((OSVERSIONINFO*) &m_osvi);

    // 设置当前语言。主要是用于 strcoll 中文拼音排序(如：combobox排序)(TODO:这一个是不是需要作成一个配置项？)
    _wsetlocale( LC_ALL, _T("chs") );

    // 初始化Gdiplus
    // 注：gdiplus会创建一个背景线程：GdiPlus.dll!BackgroundThreadProc()  + 0x59 字节	
    Image::InitGDIPlus();

    /* INITIALIZE COMMON CONTROLS, tooltip support */
    INITCOMMONCONTROLSEX iccex; 
    iccex.dwICC = ICC_WIN95_CLASSES;
    iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    InitCommonControlsEx(&iccex);

    // 创建一个用于转发消息的窗口，实现post ui message
    m_WndForwardPostMsg.Create(HWND_MESSAGE);

    // 针对layer window防止无响应时窗口变黑
    //DisableProcessWindowsGhosting();

    m_bGpuEnable = false;

    RegisterDefaultUIObject(); 
    RegisterWndClass();
}

UIApplication::~UIApplication(void)
{
    s_lAppCount--;

	// 应用程序退出日志
	UI_LOG_INFO( _T("\n\n------------  UI Quit ----------------\n"));

	this->m_ToolTipMgr.Release();  // 保证顶层窗口计数为0

#ifdef _DEBUG
	int nCount = this->m_TopWindowMgr.GetTopWindowCount();
	if (0 != nCount)   // <-- 该方法有可能还是不准，有可能窗口被销毁了，但窗口对象还没有析构
	{
		UI_LOG_WARN(_T("UI_Exit TopWindowCount=%d"),nCount);  
	}
#endif

    if (0 == s_lAppCount)
    {
	    SAFE_RELEASE(s_pLog);
        ReleaseUID3D();
		ReleaseUID2D();
    }

    ClearRegisterUIObject();

	if (m_WndForwardPostMsg.IsWindow())
	{
		m_WndForwardPostMsg.DestroyWindow();
	}
	m_skinMgr.Destroy();

	m_pUIEditor = NULL;
	SAFE_DELETE(m_pAnimateMgr)  // 如果不先调用的话，会报: 0xC0000096: Privileged instruction. （是不是与ole uninitialize 有关?)
	SAFE_DELETE(m_pGifTimerMgr);

    Image::ReleaseGDIPlus();

    if (m_bGpuEnable)
    {
        ShutdownGpuCompositor();
    }

    //	::CoUninitialize(); // do not call CoInitialize, CoInitializeEx, or CoUninitialize from the DllMain function. 
    OleUninitialize();
}

// 外部调用，共享日志指针
bool  UIApplication::LogUI(ILog* pLog)
{
	SAFE_RELEASE(s_pLog);
	if (pLog)
	{
		s_pLog = pLog;
		pLog->AddRef();

		ILogItem* pLogItem = NULL;
		pLog->GetLogItemByName(XML_LOG_UI_ID, &pLogItem);
		if (pLogItem)
		{
			pLogItem->GetCookie(&s_lUiLogCookie);
		}
	}
	return true;
}

//
//	初始化日志对象
//
//	Parameter
//		logXmlPath
//			[in]  日志配置文件的全路径
//
bool  UIApplication::LogUI(LPCTSTR szLogXmlPath)
{
	SAFE_RELEASE(s_pLog);
	if (!szLogXmlPath)
		return false;

	HRESULT hr = S_OK;
	if (NULL == s_pLog)
	{
		hr = ::CoCreateInstance(__uuidof(Log), NULL, CLSCTX_INPROC, __uuidof(ILog), (void**)&s_pLog);
		if (FAILED(hr))
			return false;
	}

	hr = s_pLog->Load((BSTR)szLogXmlPath);
	if (FAILED(hr))
	{
		SAFE_RELEASE(s_pLog);
		return false;
	}

	ILogItem* pLogItem = NULL;
	s_pLog->GetLogItemByName(XML_LOG_UI_ID, &pLogItem);
	if (pLogItem)
	{
		pLogItem->GetCookie(&s_lUiLogCookie);
	}

	return true;
}

bool  UIApplication::GetLog(ILog** ppLog)
{
	if (NULL == s_pLog || NULL == ppLog)
		return false;

	*ppLog = s_pLog;
	(*ppLog)->AddRef();
	return true;
}

HRESULT UIApplication::SetTooltipsUI(IToolTipUI* pUI)
{
// 	m_ToolTipMgr.Init(pUI);
// 	return S_OK;
    return E_NOTIMPL;
}

HRESULT UIApplication::UseInnerTooltipsUI(LPCTSTR szWndID)
{
	if (NULL == szWndID)
		return false;
#if 0 // -- 架构改造
	TooltipWindow* pWindow = new TooltipWindow;
	static_cast<CustomWindow*>(pWindow)->Create(this, String(szWndID));
	
	return this->SetTooltipsUI(pWindow);
#endif 
    return 0;
}

SkinManager& UIApplication::GetSkinMgr()
{
	return m_skinMgr;
}

ITopWindowManager* UIApplication::GetITopWindowMgr()
{
	return m_TopWindowMgr.GetITopWindowManager();
}

IAnimateManager* UIApplication::GetAnimateMgr()
{
	return m_pAnimateMgr->GetIAnimateManager();
}

SkinRes* UIApplication::GetDefaultSkinRes()
{
	return m_skinMgr.GetDefaultSkinRes();
}

ImageManager*  UIApplication::GetActiveSkinImageMgr()
{
	SkinRes* pSkinRes = GetDefaultSkinRes();
	if (NULL == pSkinRes)
		return NULL;

	return &pSkinRes->GetImageManager();
}
ImageRes*  UIApplication::GetActiveSkinImageRes()
{
	ImageManager* pImageMgr = GetActiveSkinImageMgr();
	if (NULL == pImageMgr)
		return NULL;

	return &pImageMgr->GetImageRes();
}
CursorRes*  UIApplication::GetActiveSkinCursorRes()
{
	ImageManager* pImageMgr =GetActiveSkinImageMgr();
	if (NULL == pImageMgr)
		return NULL;

	return &pImageMgr->GetCursorRes();
}
GifRes*  UIApplication::GetActiveSkinGifRes()
{
	ImageManager* pImageMgr = GetActiveSkinImageMgr();
	if (NULL == pImageMgr)
		return NULL;

	return &pImageMgr->GetGifRes();
}
FontManager*  UIApplication::GetActiveSkinFontMgr()
{
	SkinRes* pSkinRes = GetDefaultSkinRes();
	if (NULL == pSkinRes)
		return NULL;

	return &pSkinRes->GetFontManager();
}
FontRes*  UIApplication::GetActiveSkinFontRes()
{
	FontManager* pFontMgr = GetActiveSkinFontMgr();
	if (NULL == pFontMgr)
		return NULL;

	return &pFontMgr->GetFontRes();
}
ColorManager*  UIApplication::GetActiveSkinColorMgr()
{
	SkinRes* pSkinRes = GetDefaultSkinRes();
	if (NULL == pSkinRes)
		return NULL;

	return &pSkinRes->GetColorManager();
}

ColorRes*  UIApplication::GetActiveSkinColorRes()
{
	ColorManager* pColorMgr = GetActiveSkinColorMgr();
	if (NULL == pColorMgr)
		return NULL;

	return &pColorMgr->GetColorRes();
}
StyleManager*  UIApplication::GetActiveSkinStyleMgr()
{
	SkinRes* pSkinRes = GetDefaultSkinRes();
	if (NULL == pSkinRes)
		return NULL;

	return &pSkinRes->GetStyleManager();
}

StyleRes*  UIApplication::GetActiveSkinStyleRes()
{
	StyleManager* pStyleMgr = GetActiveSkinStyleMgr();
	if (NULL == pStyleMgr)
		return NULL;

	return &pStyleMgr->GetStyleRes();
}

LayoutManager*  UIApplication::GetActiveSkinLayoutMgr()
{
	SkinRes* pSkinRes = GetDefaultSkinRes();
	if (NULL == pSkinRes)
		return NULL;

	return &pSkinRes->GetLayoutManager();
}

LayoutRes*  UIApplication::GetActiveSkinLayoutRes()
{
	LayoutManager* pLayoutMgr = GetActiveSkinLayoutMgr();
	if (NULL == pLayoutMgr)
		return NULL;

	return &pLayoutMgr->GetLayoutRes();
}

//
//	一个空的窗口过程，因为UI这个窗口类的窗口过程最终要被修改成为一个类的成员函数，
//  因此这里的窗口过程只是用来填充WNDCLASS参数。
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return ::DefWindowProc( hWnd, message, wParam, lParam );
}

/*
**	[private] void  RegisterWndClass()
**
**	Parameters
**
**	Return
**		NA
**
**	Remark
**		Window对象的窗口使用的窗口类就是在这里注册的。
**
**	See Also
*/
void UIApplication::RegisterWndClass()
{
	WNDCLASSEX wcex;
	
	// 注册UI普通窗口类
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= /*CS_HREDRAW | CS_VREDRAW |*/CS_DBLCLKS;  // <-- 分层窗口最大化时收不到redraw消息，因此直接在OnSize中刷新
	wcex.lpfnWndProc	= WindowBase::StartWindowProc;//WndProc;   改用windows默认的窗口过程
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= 0;//::AfxGetInstanceHandle();;
	wcex.hIcon			= 0;//LoadIcon(0, MAKEINTRESOURCE(IDR_MAINFRAME));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1); // 2014.5.21 将背景置为NULL，解决aero磨砂窗口从最小化还原时，会先显示白色背景，再刷新内容的闪烁问题（触发了WM_NCCALCSIZE内部估计会做COPY操作）
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= WND_CLASS_NAME;
	wcex.hIconSm		= 0;//LoadIcon(0, MAKEINTRESOURCE(IDR_MAINFRAME));
	RegisterClassEx(&wcex);

	// 注册用于实现动画的窗口类
	wcex.lpszClassName  = WND_ANIMATE_CLASS_NAME;
	wcex.lpfnWndProc    = ::DefWindowProc;
	RegisterClassEx(&wcex);


	// 注册用于实现菜单，弹出式列表框的窗口(带阴影/不带阴影)
	wcex.lpszClassName  = WND_POPUP_CONTROL_SHADOW_NAME;
	wcex.style          = CS_DROPSHADOW;
	wcex.lpfnWndProc    = WindowBase::StartWindowProc;;
	RegisterClassEx(&wcex);

	wcex.lpszClassName  = WND_POPUP_CONTROL_NAME;
	wcex.style          = 0;
	wcex.lpfnWndProc    = WindowBase::StartWindowProc;;
	RegisterClassEx(&wcex);

	// 拖拽时显示的图片窗口
	wcex.lpszClassName  = WND_DRAGBITMAPWND_CLASS_NAME;
	wcex.style          = 0;
	wcex.lpfnWndProc    = DefWindowProc;
	RegisterClassEx(&wcex);

}

bool UIApplication::IsUnderXpOS()
{
	bool bUnderXpOs = true;;
	if (VER_PLATFORM_WIN32_NT == m_osvi.dwPlatformId)
	{
		if (m_osvi.dwMajorVersion >= 6)
		{
			bUnderXpOs = false;
		}
	}
	else
	{
		bUnderXpOs = false;
	}
	return bUnderXpOs;
}

bool UIApplication::IsVistaOrWin7etc()
{
	bool bHighThanVista = true;;
	if (VER_PLATFORM_WIN32_NT == m_osvi.dwPlatformId)
	{
		if (m_osvi.dwMajorVersion < 6)
		{
			bHighThanVista = false;
		}
	}
	else
	{
		bHighThanVista = false;
	}
	return bHighThanVista;
}

// bool UIApplication::IsUIObjectAvailable(IMessage* p)
// {	
// 	if (NULL == p)
// 		return false;
// 
// 	list<IMessage*>::iterator iter = std::find(m_aliveUIObject.begin(), m_aliveUIObject.end(), p);
// 	if (iter == m_aliveUIObject.end())
// 		return false;
// 
// 	return true;
// }
// 
// AddUIObject在控件增加，尤其是listctrl中时，效率急剧降低
// HRESULT UIApplication::AddUIObject(IMessage* p)
// {
// 	if (NULL == p)
// 		return E_INVALIDARG;
// 
// 	list<IMessage*>::iterator iter = std::find(m_aliveUIObject.begin(), m_aliveUIObject.end(), p);
// 	if (iter == m_aliveUIObject.end())
// 	{
// 		m_aliveUIObject.push_back(p);
// 		return S_OK;
// 	}
// 
// 	return E_FAIL;
// }
// HRESULT UIApplication::RemoveUIObject(IMessage* p)
// {
// 	if (NULL == p)
// 		return E_FAIL;
// 
// 	list<IMessage*>::iterator iter = std::find(m_aliveUIObject.begin(), m_aliveUIObject.end(), p);
// 	if (iter != m_aliveUIObject.end())
// 	{
// 		m_aliveUIObject.erase(iter);
// 		return S_OK;
// 	}
// 
// 	return E_FAIL;
// }

bool  UIApplication::GetSkinTagParseFunc(LPCTSTR szTag, pfnParseSkinTag* pFunc)
{
    if (NULL == szTag || NULL == pFunc)
        return false;

    UISKINTAGPARSE_DATA::iterator iter = m_mapSkinTagParseData.find(szTag);
    if (iter == m_mapSkinTagParseData.end())
        return false;

    *pFunc = iter->second;
    return true;
}

bool  UIApplication::RegisterControlTagParseFunc(LPCTSTR szTag, pfnParseControlTag func)
{
    if (NULL == szTag || NULL == func)
        return false;

    m_mapControlTagParseData[szTag] = func;
    return true;
}

bool  UIApplication::GetControlTagParseFunc(LPCTSTR szTag, pfnParseControlTag* pFunc)
{
    if (NULL == szTag || NULL == pFunc)
        return false;

    UICONTROLTAGPARSE_DATA::iterator iter = m_mapControlTagParseData.find(szTag);
    if (iter == m_mapControlTagParseData.end())
        return false;

    *pFunc = iter->second;
    return true;
}

//
//	为了实现UI对象的创建（从字符串来创建其对应的类），在app类中保存了所有UI对象的创建信息。
//
//	注: 如果仅仅采用在UICreateObject中添加映射列表，无法处理第三方实现一个UI对象的情况，因些
//      必须将该映射列表保存为动态数组。当第三方实现了一个UI类时，向app来注册其创建信息。
//

bool UIApplication::RegisterUIObjectCreateData(LPCTSTR bstrXmlName, 
                                                  LPCTSTR szCategory, UINT nObjType, 
                                                  REFCLSID guid, pfnUICreateInstancePtr pfun)
{
	if (NULL == pfun || NULL == bstrXmlName)
	{
		UI_LOG_WARN(_T("invalid arg. name=%s, pfun=%x"), bstrXmlName, pfun);
		return false;
	}
	
	int nSize = (int)m_vecUICreateData.size();
	for (int i = 0; i < nSize; i++)
	{
		UIOBJ_CREATE_INFO* pData = m_vecUICreateData[i];
		if (pData && pData->m_strXmlName == bstrXmlName)
		{
			UI_LOG_WARN(_T("register duplicate. name=%s, pfun=%x"), bstrXmlName, pfun);
			return false;
		}
	}

	UIOBJ_CREATE_INFO* pData = new UIOBJ_CREATE_INFO;
	pData->m_strXmlName = bstrXmlName;
    pData->m_nObjType = nObjType;
    memcpy(&pData->m_guid, &guid, sizeof(GUID));
	pData->m_func = pfun;
    if (szCategory)
        pData->m_strCategory = szCategory;
	
	m_vecUICreateData.push_back(pData);

    LPOLESTR lpolestr = NULL;
    StringFromIID(guid, &lpolestr);
    UI_LOG_DEBUG(_T(" %s %s @ 0x%08X"), bstrXmlName, lpolestr, pfun);
    CoTaskMemFree(lpolestr);
      
	return true;
}

bool UIApplication::GetUICreateInstanceFuncPtr(LPCTSTR bstrXmlName, pfnUICreateInstancePtr* pOut)
{
	if (NULL == bstrXmlName || NULL == pOut)
		return false;
	
    UIOBJ_CREATE_DATA::iterator iter = m_vecUICreateData.begin();
	for (; iter != m_vecUICreateData.end(); iter++)
	{
		UIOBJ_CREATE_INFO* pData = *iter;
		if (pData && pData->m_strXmlName == bstrXmlName)
		{
			*pOut = pData->m_func;
			return true;
		}
	}
	return false;
}

bool UIApplication::GetUICreateInstanceFuncPtr(REFCLSID guid, pfnUICreateInstancePtr* pOut)
{
    if (NULL == pOut)
        return false;

    int nSize = (int)m_vecUICreateData.size();
    for (int i = 0; i < nSize; i++)
    {
        UIOBJ_CREATE_INFO* pData = m_vecUICreateData[i];
        if (NULL == pData)
            continue;

        if (::IsEqualIID(guid, pData->m_guid))
        {
            *pOut = pData->m_func;
            return true;
        }
    }

    LPOLESTR pText = NULL;
    StringFromIID(guid, &pText);
    if (pText)
    {
        UI_LOG_WARN(_T("get func ptr failed. pGuid=%s"), pText);
        ::CoTaskMemFree(pText);
    }
    return false;
}

void  UIApplication::ClearRegisterUIObject()
{
#define vec_clear(type, var)                  \
    {                                         \
        type::iterator iter = var.begin();    \
        for ( ; iter != var.end(); iter++ )   \
            SAFE_DELETE(*iter);               \
        var.clear();                          \
    }

    vec_clear(UIOBJ_CREATE_DATA,            m_vecUICreateData);
    vec_clear(UIRENDERBASE_CREATE_DATA,     m_vecUIRenderBaseCreateData);
    vec_clear(UITEXTRENDERBASE_CREATE_DATA, m_vecUITextRenderBaseCreateData);
    vec_clear(UILAYOUT_CREATE_DATA,         m_vecUILayoutCreateData);
}

void UIApplication::RegisterDefaultUIObject()
{
	LPCTSTR  CATEGORY_CONTROL = _T("Kernel/Control");
	LPCTSTR  CATEGORY_CONTAINER = _T("Kernel/Container");
	LPCTSTR  CATEGORY_WINDOW = _T("Kernel/Window");

    REGISTER_UI_OBJECT (this, CATEGORY_WINDOW, Window);
    REGISTER_UI_OBJECT (this, CATEGORY_WINDOW, CustomWindow);
    REGISTER_UI_OBJECT (this, CATEGORY_CONTROL, HwndHost);
	REGISTER_UI_OBJECT (this, CATEGORY_CONTAINER, Panel);
    REGISTER_UI_OBJECT (this, CATEGORY_CONTAINER, ScrollPanel);

    //REGISTER_UI_RENDERBASE(this, NullRender) -- 2015.4.1过期，不再使用
    REGISTER_UI_RENDERBASE(this, ColorRender);
    REGISTER_UI_RENDERBASE(this, SysColorRender);
    REGISTER_UI_RENDERBASE(this, GradientRender);
    REGISTER_UI_RENDERBASE(this, ColorListRender);
    REGISTER_UI_RENDERBASE(this, ImageRender);
    REGISTER_UI_RENDERBASE(this, ImageListItemRender);
    REGISTER_UI_RENDERBASE(this, ImageListRender);

    REGISTER_UI_TEXTRENDERBASE(this, SimpleTextRender)
    REGISTER_UI_TEXTRENDERBASE(this, ColorListTextRender)
    REGISTER_UI_TEXTRENDERBASE(this, FontColorListTextRender)
    REGISTER_UI_TEXTRENDERBASE(this, ContrastColorTextRender)
    REGISTER_UI_TEXTRENDERBASE(this, ContrastColorListTextRender)
        
    REGISTER_UI_LAYOUT(this, _T("canvas"), LAYOUT_TYPE_CANVAS, CanvasLayout::CreateInstance)
    REGISTER_UI_LAYOUT(this, _T("stack"),  LAYOUT_TYPE_STACK,  StackLayout::CreateInstance)
    REGISTER_UI_LAYOUT(this, _T("card"),   LAYOUT_TYPE_CARD,   CardLayout::CreateInstance)
    REGISTER_UI_LAYOUT(this, _T("grid"),   LAYOUT_TYPE_GRID,   GridLayout::CreateInstance)
    REGISTER_UI_LAYOUT(this, _T("average"),   LAYOUT_TYPE_AVERAGE,   AverageLayout::CreateInstance)

    m_mapSkinTagParseData[XML_IMG] = ImageManager::UIParseImageTagCallback;
    m_mapSkinTagParseData[XML_COLOR] = ColorManager::UIParseColorTagCallback;
    m_mapSkinTagParseData[XML_FONT] = FontManager::UIParseFontTagCallback;
    m_mapSkinTagParseData[XML_STYLE] = StyleManager::UIParseStyleTagCallback;
    m_mapSkinTagParseData[XML_LAYOUT] = LayoutManager::UIParseLayoutTagCallback;
	m_mapSkinTagParseData[XML_LAYOUTCONFIG] = LayoutManager::UIParseLayoutConfigTagCallback;
    m_mapSkinTagParseData[XML_INCLUDE] = SkinParseEngine::UIParseIncludeTagCallback;
}

// 用于编辑器中调整控件库的依赖
void  UIApplication::RestoreRegisterUIObject()
{
    ClearRegisterUIObject();
    RegisterDefaultUIObject();
}

IObject* UIApplication::CreateInstanceByName(LPCTSTR szXmlName, ISkinRes* pSkinRes)
{
	if (!szXmlName)
		return NULL;

	pfnUICreateInstancePtr  funcptr;
	if (false == this->GetUICreateInstanceFuncPtr(szXmlName, &funcptr))
	{
		UI_LOG_ERROR(_T("GetUICreateInstanceFuncPtr Failed. name=%s"), szXmlName);
		return false;
	}

    IObject* pReturn = NULL;
	funcptr(m_pUIApplication, pSkinRes, (void**)&pReturn);
	return pReturn;
}

IObject* UIApplication::CreateInstanceByClsid(REFCLSID clsid, ISkinRes* pSkinRes)
{
    pfnUICreateInstancePtr  funcptr;
    if (false == this->GetUICreateInstanceFuncPtr(clsid, &funcptr))
    {
        LPOLESTR lpstr = NULL;
        StringFromIID(clsid, &lpstr);
        UI_LOG_ERROR(_T("%s GetUICreateInstanceFuncPtr Failed. clsid=%s"), lpstr);
        CoTaskMemFree(lpstr);

        return NULL;
    }

	IObject* pReturn = NULL;
    funcptr(m_pUIApplication, pSkinRes, (void**)&pReturn);
	return pReturn;
}

bool  UIApplication::RegisterUIRenderBaseCreateData(LPCTSTR bstrName, int nType, pfnUICreateRenderBasePtr pfunc)
{
    if (NULL == bstrName || NULL == pfunc)
        return false;

    String strName(bstrName);

    UIRENDERBASE_CREATE_INFO* pInfo = new UIRENDERBASE_CREATE_INFO;
    pInfo->m_func = pfunc;
    pInfo->m_nRenderType = nType;
    pInfo->m_strName = strName;
    m_vecUIRenderBaseCreateData.push_back(pInfo);

    UI_LOG_DEBUG(_T("%s, type=%d, ctrl=%d @ 0x%08X"), bstrName, nType, pfunc);
    return true;
}
bool  UIApplication::CreateRenderBaseByName(LPCTSTR strName, IObject* pObject, IRenderBase** ppOut)
{
    if (NULL == strName || NULL == pObject || NULL == ppOut)
        return false;

    UIRENDERBASE_CREATE_DATA::iterator iter = m_vecUIRenderBaseCreateData.begin();
    for ( ; iter != m_vecUIRenderBaseCreateData.end(); iter++ )
    {
        UIRENDERBASE_CREATE_INFO* pData = *iter;
        if (NULL == pData)
            continue;

        if (pData->m_strName != strName)
            continue;

        // 废弃
//      if (-1 != pData->m_nControlType)
//      {
//          if (pObject->GetObjectExtentType() != pData->m_nControlType)
//              continue;
//      }
// 		
//      if (-1 != pData->m_nControlSubType)
//      {
//          int  nStylyEx = pObject->GetStyleEx();
//          if (GETCONTROLSUBTYPE(nStylyEx) != pData->m_nControlSubType)
//              continue;
//      }

        HRESULT hr = pData->m_func((void**)ppOut);
        if (SUCCEEDED(hr) && NULL != *ppOut)
        {
            (*ppOut)->SetObject(m_pUIApplication, pObject);
            (*ppOut)->Init();
            (*ppOut)->SetType((RENDER_TYPE)pData->m_nRenderType);
			return true;
        }
        
        return false;
    }

    UI_LOG_WARN(_T("Create Failed. Name=%s"), strName);
    return false;
}
bool  UIApplication::CreateRenderBase(int nType, IObject* pObject, IRenderBase** ppOut)
{
    if (NULL == ppOut)
        return false;

    UIRENDERBASE_CREATE_DATA::iterator iter = m_vecUIRenderBaseCreateData.begin();
    for ( ; iter != m_vecUIRenderBaseCreateData.end(); iter++ )
    {
        UIRENDERBASE_CREATE_INFO* pData = *iter;
        if (NULL == pData)
            continue;

        if (pData->m_nRenderType != nType)
            continue;

        HRESULT hr = pData->m_func((void**)ppOut);
        if (SUCCEEDED(hr) && NULL != *ppOut)
        {
            (*ppOut)->SetObject(m_pUIApplication, pObject);
            (*ppOut)->Init();
            (*ppOut)->SetType((RENDER_TYPE)nType);
			return true;
        }
        
        return false;
    }

    UI_LOG_WARN(_T("Create Failed. Type=%d"), nType);
    return false;
}

// 根据类型获取对应的xml name
LPCTSTR  UIApplication::GetRenderBaseName(int nType)
{
    UIRENDERBASE_CREATE_DATA::iterator iter = m_vecUIRenderBaseCreateData.begin();
    for ( ; iter != m_vecUIRenderBaseCreateData.end(); ++iter )
    {
        UIRENDERBASE_CREATE_INFO* pData = *iter;
        if (NULL == pData)
            continue;

        if (pData->m_nRenderType != nType)
            continue;

        return pData->m_strName.c_str();
    }

    return NULL;
}

void  UIApplication::EnumRenderBaseName(pfnEnumRenderBaseNameCallback callback, WPARAM wParam, LPARAM lParam)
{
    UIRENDERBASE_CREATE_DATA::iterator iter = m_vecUIRenderBaseCreateData.begin();
    for ( ; iter != m_vecUIRenderBaseCreateData.end(); ++iter )
    {
        UIRENDERBASE_CREATE_INFO* pData = *iter;
        if (NULL == pData)
            continue;

        callback(pData->m_strName.c_str(), wParam, lParam);
    }
}

bool  UIApplication::RegisterUITextRenderBaseCreateData(LPCTSTR szName, int nType, pfnUICreateTextRenderBasePtr pfunc)
{
    if (NULL == szName || NULL == pfunc)
        return false;

    String strName(szName);

    UITEXTRENDERBASE_CREATE_INFO* pInfo = new UITEXTRENDERBASE_CREATE_INFO;
    pInfo->m_func = pfunc;
    pInfo->m_nRenderType = nType;
    pInfo->m_strName = strName;
    m_vecUITextRenderBaseCreateData.push_back(pInfo);

    UI_LOG_DEBUG(_T("%s   @ 0x%08X"), szName,  pfunc);
    return true;
}
bool  UIApplication::CreateTextRenderBaseByName(LPCTSTR bstrName, IObject* pObject, ITextRenderBase** ppOut)
{
    if (NULL == bstrName || NULL == pObject || NULL == ppOut)
        return false;

    UITEXTRENDERBASE_CREATE_DATA::iterator iter = m_vecUITextRenderBaseCreateData.begin();
    for ( ; iter != m_vecUITextRenderBaseCreateData.end(); iter++ )
    {
        UITEXTRENDERBASE_CREATE_INFO* pData = *iter;
        if (NULL == pData)
            continue;

        if (pData->m_strName != bstrName)
            continue;

        // 废弃
//      if (-1 != pData->m_nControlType)
//      {
//          if (pObject->GetObjectExtentType() != pData->m_nControlType)
//              continue;
//      }
//      if (-1 != pData->m_nControlSubType)
//      {
//          int  nStylyEx = pObject->GetStyleEx();
//          if (GETCONTROLSUBTYPE(nStylyEx) != pData->m_nControlSubType)
//              continue;
//      }

        HRESULT hr = pData->m_func((void**)ppOut);
        if (SUCCEEDED(hr) && NULL != *ppOut)
        {
            (*ppOut)->SetObject(m_pUIApplication, pObject);
            (*ppOut)->Init();
            (*ppOut)->SetType((TEXTRENDER_TYPE)pData->m_nRenderType);
			return true;
        }

        return false;
    }

    UI_LOG_WARN(_T("Create Failed. Name=%s"), bstrName);
    return false;
}
bool  UIApplication::CreateTextRenderBase(int nType, IObject* pObject, ITextRenderBase** ppOut)
{
    if (/*NULL == pObject ||*/ NULL == ppOut)
        return false;

    UITEXTRENDERBASE_CREATE_DATA::iterator iter = m_vecUITextRenderBaseCreateData.begin();
    for ( ; iter != m_vecUITextRenderBaseCreateData.end(); iter++ )
    {
        UITEXTRENDERBASE_CREATE_INFO* pData = *iter;
        if (NULL == pData)
            continue;

        if (pData->m_nRenderType != nType)
            continue;

        HRESULT hr = pData->m_func((void**)ppOut);
        if (SUCCEEDED(hr) && NULL != *ppOut)
        {
            (*ppOut)->SetObject(m_pUIApplication, pObject);
            (*ppOut)->Init();
            (*ppOut)->SetType((TEXTRENDER_TYPE)nType);
			return true;
        }

        return false;
    }

    UI_LOG_WARN(_T("Create Failed. Type=%d"), nType);
    return false;
}
LPCTSTR  UIApplication::GetTextRenderBaseName(int nType)
{
    UITEXTRENDERBASE_CREATE_DATA::iterator iter = m_vecUITextRenderBaseCreateData.begin();
    for ( ; iter != m_vecUITextRenderBaseCreateData.end(); iter++ )
    {
        UITEXTRENDERBASE_CREATE_INFO* pData = *iter;
        if (NULL == pData)
            continue;

        if (pData->m_nRenderType != nType)
            continue;

        return pData->m_strName.c_str();
    }

    return NULL;
}

void  UIApplication::EnumTextRenderBaseName(pfnEnumTextRenderBaseNameCallback callback, WPARAM wParam, LPARAM lParam)
{
    UITEXTRENDERBASE_CREATE_DATA::iterator iter = m_vecUITextRenderBaseCreateData.begin();
    for ( ; iter != m_vecUITextRenderBaseCreateData.end(); iter++ )
    {
        UITEXTRENDERBASE_CREATE_INFO* pData = *iter;
        if (NULL == pData)
            continue;

        callback(pData->m_strName.c_str(), wParam, lParam);
    }
}

bool  UIApplication::RegisterLayoutCreateData(LPCTSTR szName, int nType, pfnUICreateLayoutPtr pfun)
{
    if (NULL == pfun || NULL == szName)
    {
        UI_LOG_WARN(_T("invalid arg. name=%s, pfun=%x"), szName, pfun);
        return false;
    }

    int nSize = (int)m_vecUILayoutCreateData.size();
    for (int i = 0; i < nSize; i++)
    {
        UILAYOUT_CREATE_INFO* pData = m_vecUILayoutCreateData[i];
        if (pData && (pData->m_strName == szName || pData->m_nLayoutType == nType))
        {
            UI_LOG_WARN(_T("register duplicate. name=%s, type=%d, pfun=%x"), szName, nType, pfun);
            return false;
        }
    }

    UILAYOUT_CREATE_INFO* pData = new UILAYOUT_CREATE_INFO;
    pData->m_strName = szName;
    pData->m_nLayoutType = nType;
    pData->m_func = pfun;

    m_vecUILayoutCreateData.push_back(pData);
    return true;
}
bool  UIApplication::CreateLayout(int nType, IObject* pObject, ILayout** ppOut)
{
    if (NULL == ppOut)
        return false;

    pfnUICreateLayoutPtr  pFunc = NULL;
    UILAYOUT_CREATE_DATA::iterator iter = m_vecUILayoutCreateData.begin();
    for ( ; iter != m_vecUILayoutCreateData.end(); iter++ )
    {
        UILAYOUT_CREATE_INFO* pData = *iter;
        if (pData && pData->m_nLayoutType == nType)
        {
            pFunc = pData->m_func;
            break;
        }
    }

    if (NULL == pFunc)
        return false;

    pFunc(pObject, ppOut);
    return true;
}
bool  UIApplication::CreateLayoutByName(LPCTSTR szName, IObject* pObject, bool bCreateDefault, ILayout** ppOut)
{
    if (NULL == ppOut)
        return false;

    pfnUICreateLayoutPtr  pFunc = NULL;
    if (szName)  // 为NULL有可能是没有配置layout属性，默认创建一个canvas布局
    {
        UILAYOUT_CREATE_DATA::iterator iter = m_vecUILayoutCreateData.begin();
        for ( ; iter != m_vecUILayoutCreateData.end(); iter++ )
        {
            UILAYOUT_CREATE_INFO* pData = *iter;
            if (pData && pData->m_strName == szName)
            {
                pFunc = pData->m_func;
                break;
            }
        }
    }

    if (NULL == pFunc && bCreateDefault && m_vecUILayoutCreateData.size() > 0)
        pFunc = m_vecUILayoutCreateData[0]->m_func;

    if (NULL == pFunc)
        return false;

    pFunc(pObject, ppOut);
    return true;
}

void  UIApplication::EnumLayoutType(pfnEnumLayoutTypeCallback callback, WPARAM wParam, LPARAM lParam)
{
    if (!callback)
        return;

    UILAYOUT_CREATE_DATA::iterator iter = m_vecUILayoutCreateData.begin();
    for ( ; iter != m_vecUILayoutCreateData.end(); iter++ )
    {
        if (!callback((*iter)->m_nLayoutType, (*iter)->m_strName.c_str(), wParam, lParam))
            break;
    }
}

BOOL UIApplication::IsDialogMessage(MSG* pMsg)
{
	if (NULL == pMsg)
		return FALSE;

	if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		(pMsg->message < (WM_MOUSEFIRST+1) || pMsg->message > WM_MOUSELAST))
		return FALSE;

//	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB )
	{
		// 获取这个消息的窗口所在的顶层窗口，因为导航是针对是整个顶层窗口进行的
        // 2013.6.20 屏蔽掉这些代码。存在子窗口是UI窗口，但顶层窗口是普通窗口的情况
 		HWND hWndTop = pMsg->hwnd;
// 		while (1)
// 		{
// 			if (NULL == hWndTop)
// 				return FALSE;
// 
// 			LONG lStyle = ::GetWindowLong(hWndTop, GWL_STYLE);
// 			if (lStyle & WS_CHILD)
// 			{
// 				hWndTop = ::GetParent(hWndTop);
// 			}
// 			else
// 			{
// 				break;
// 			}
// 		}

		// 判断这个窗口是否属性UI管理的一个顶层窗口
		IWindowBase* pIWindow = m_TopWindowMgr.GetWindowBase(hWndTop);
        if (NULL == pIWindow)
            return FALSE;

        WindowBase* pWindow = pIWindow->GetWindowBaseImpl();
        if (!pWindow)
            return FALSE;

        return pWindow->GetMouseMgr()->IsDialogMessage(pMsg);
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
//
// http://dsdm.bokee.com/6033955.html  如何正确使用PeekMessage
// 这里要注意的是多重模态的问题，如何保证正常退出
//
//////////////////////////////////////////////////////////////////////////
// 2014.2.27 libo
// 注意，不要使用MsgWaitForMultipleObjects -- QS_ALLEVENTS标志，会出现系统卡顿现象，例如安装了底层键盘钩子时，一输入就卡了
//
// HHOOK  g_hKeyBoardHook = NULL;
// LRESULT CALLBACK  LLKeyboardProc(int code, WPARAM wParam, LPARAM lParam)
// {
//     return 0;
//     return CallNextHookEx(g_hKeyBoardHook, code, wParam, lParam);
// }
// hKeyBoardHook = ::SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)KeyboardProc, g_hInstance, NULL);
// 
//////////////////////////////////////////////////////////////////////////


//
//  WaitMessage 使用需知：
//  A pretty hard rule for WaitMessage() is that the message queue should be empty before you call it. 
//  If it isn't empty then any messages left in the queue are marked as "seen" and WaitMessage() ignores them.
// 
//  如果没有新的消息，WaitMessage不会返回。PeekMessage/GetMessage等函数会将现有
//  消息队列中的消息标识为旧消息，因此
//  PostMessage(xxx);
//  PeekMessage(PM_NOREMOVE);
//  WaitMessage();  这里并不会立即返回，xxx消息已被标识为旧消息。除非有一个新的
//  消息到来才能使WaitMessage返回。
//
void  UIApplication::MsgHandleLoop(bool* pbQuitLoopRef)
{
	DWORD    dwRet = 0;
    DWORD&   nCount = m_WaitForHandlesMgr.m_nHandleCount;
    HANDLE*& pHandles = m_WaitForHandlesMgr.m_pHandles;
	MSG      msg;

    // 会传递pbQuitLoopRef参数的，有可能是Modal类型的菜单，这种情况下需要更多的条件判断
    // 因此单独用一个分支来优化
    if (pbQuitLoopRef)
    {
        bool   bExit = false;
        bool&  bQuitRef = (pbQuitLoopRef==NULL ? bExit : *pbQuitLoopRef);
	    while (false == bQuitRef)
	    {
		    dwRet = ::MsgWaitForMultipleObjects(nCount, pHandles, FALSE, INFINITE, QS_ALLINPUT) - WAIT_OBJECT_0;
		    if (nCount > 0 && dwRet < nCount)
		    {
			    m_WaitForHandlesMgr.Do(pHandles[dwRet]);
		    }
		    else
		    {
			    while (::PeekMessage(&msg, NULL, 0,0, PM_NOREMOVE))
			    {
				    if (WM_QUIT == msg.message)  // 一直传递给最外的Loop，避免在内部Loop中将该消息吃掉了
					    return;

                    if (m_MsgFilterMgr.PreTranslateMessage(&msg))  // 有可能在PreTranslateMessage中修改了pbQuitLoopRef
                    {
                        PeekMessage(&msg, NULL, 0,0, PM_REMOVE);   // 该消息已被处理过了，移除
                        continue;
                    }

                    if (bQuitRef)     // 可能的场景是弹出模态菜单后，然后在主窗口的关闭按钮上点了一下，然后菜单的PopupMenuWindow::PreTranslateMessage就要求退出了
                        return;  // 如果这时还继续处理，就会导致主窗口被销毁，当前用于弹出菜单的堆栈对象也被销毁了

                    PeekMessage(&msg, NULL, 0,0, PM_REMOVE);
                    if (FALSE == this->IsDialogMessage(&msg))
                    {
                        ::TranslateMessage(&msg);
                        ::DispatchMessage(&msg);
                    }

                    if (bQuitRef)
                        return;
			    }
		    }
	    }
    }
    else
    {
        while (1)
        {
            dwRet = ::MsgWaitForMultipleObjects(nCount, pHandles, FALSE, INFINITE, QS_ALLINPUT) - WAIT_OBJECT_0;
            if (nCount > 0 && dwRet < nCount)
            {
                m_WaitForHandlesMgr.Do(pHandles[dwRet]);
            }
            else
            {
                while (::PeekMessage(&msg, NULL, 0,0, PM_REMOVE))
                {
                    if (WM_QUIT == msg.message)  
                    {
                        ::PostMessage(NULL, WM_QUIT, 0, 0);  // 一直传递给最外的Loop，避免在内部Loop中将该消息吃掉了
                        return;
                    }

                    if (FALSE == m_MsgFilterMgr.PreTranslateMessage(&msg) && 
                        FALSE == this->IsDialogMessage(&msg)) 
                    {
                        ::TranslateMessage(&msg);                                      
                        ::DispatchMessage(&msg);
                    }
                }
            }
        }
    }
	return;
}


bool  UIApplication::ShowToolTip(TOOLTIPITEM* pItem)
{
	bool bRet = m_ToolTipMgr.Show(pItem);
    return bRet;
}
void  UIApplication::HideToolTip()
{
	m_ToolTipMgr.Hide();
}

HWND  UIApplication::GetForwardPostMessageWnd()
{
	return m_WndForwardPostMsg.m_hWnd;
}

void UIApplication::LoadUIObjectListToToolBox()
{
    if (!m_pUIEditor)
        return;

    UIOBJ_CREATE_DATA::iterator iter = m_vecUICreateData.begin();
    for (; iter != m_vecUICreateData.end(); iter++)
    {
        m_pUIEditor->OnToolBox_AddObject((*iter)->m_strXmlName.c_str(), (*iter)->m_strCategory.c_str(), (*iter)->m_nObjType);
    }
}

// 加载UI3D.dll
HMODULE  UIApplication::GetUID2DModule()
{
    if (s_hUID2D)
        return s_hUID2D;
#if 0
    TCHAR  szPath[MAX_PATH] = {0};
    Util::GetAppPath_(g_hInstance, szPath);
    _tcscat(szPath, _T("UI3D.dll"));

    s_hUID2D = LoadLibrary(szPath);

    if (!s_hUID2D)
    {
        UI_LOG_INFO(_T("%s Load UI3D.dll Failed. Error code = %d"), FUNC_NAME, GetLastError());
    }
    else
    {
        funcUI3D_InitD2D pFunc = (funcUI3D_InitD2D)GetProcAddress(s_hUID2D, NAME_UI3D_InitD2D);
        if (pFunc)
            pFunc(m_pUIApplication);
        else
            UIASSERT(0);
    }
#endif
	return s_hUID2D;
}

HMODULE  UIApplication::GetUID3DModule()
{
	if (s_hUID3D)
		return s_hUID3D;
#if 0
	TCHAR  szPath[MAX_PATH] = {0};
	Util::GetAppPath_(g_hInstance, szPath);
	_tcscat(szPath, _T("UI3D.dll"));

	s_hUID3D = LoadLibrary(szPath);

	if (!s_hUID3D)
	{
		UI_LOG_INFO(_T("%s Load UI3D.dll Failed. Error code = %d"), FUNC_NAME, GetLastError());
	}
	else
	{
		funcUI3D_InitD3D pFunc = (funcUI3D_InitD3D)GetProcAddress(s_hUID3D, NAME_UI3D_InitD3D);
		if (pFunc)
			pFunc(m_pUIApplication);
		else
			UIASSERT(0);
	}
#endif
	return s_hUID3D;
}

// 卸载UI3D.dll
void  UIApplication::ReleaseUID2D()
{
    if (s_hUID2D)
    {
#if 0
        funcUI3D_ReleaseD2D pFunc = (funcUI3D_ReleaseD2D)GetProcAddress(s_hUID2D, NAME_UI3D_ReleaseD2D);
        if (pFunc)
            pFunc();
        else
            UIASSERT(0);
#endif
        FreeLibrary(s_hUID2D);
        s_hUID2D = NULL;
    }
}
void UIApplication::ReleaseUID3D()
{
	if (s_hUID3D)
	{
#if 0
		funcUI3D_ReleaseD3D pFunc = (funcUI3D_ReleaseD3D)GetProcAddress(s_hUID3D, NAME_UI3D_ReleaseD3D);
		if (pFunc)
			pFunc();
		else
			UIASSERT(0);
#endif
		FreeLibrary(s_hUID3D);
		s_hUID3D = NULL;
	}
}

bool  UIApplication::IsGpuCompositeEnable() 
{ 
    return m_bGpuEnable;    
}
bool  UIApplication::EnableGpuComposite()
{
    if (m_bGpuEnable)
        return true;

    if (IsVistaOrWin7etc() && 0 == StartupGpuCompositor())
    {
        UI_LOG_INFO(TEXT("GpuCompositor Enable."));

        m_bGpuEnable = true;
        return true;
    }
    else
    {
        UI_LOG_INFO(TEXT("GpuCompositor Startup Failed."));
    }

    return false;
}