#ifndef UIAPPLICATION_H_369FA8A6_A4A8_47a0_9E84_082B3AABA2A1
#define UIAPPLICATION_H_369FA8A6_A4A8_47a0_9E84_082B3AABA2A1

#include "UISDK\Kernel\Src\Animate\animatemgr.h"
#include "UISDK\Kernel\Src\Helper\topwindow\topwindowmanager.h"
#include "UISDK\Kernel\Src\Helper\tooltip\tooltipmanager.h"
#include "UISDK\Kernel\Src\Helper\msg\msghelper.h"
#include "UISDK\Kernel\Src\Resource\skinmanager.h"
#include "UISDK\Kernel\Src\Helper\MouseKeyboard\mousemanager.h"


namespace UI
{
class GifTimerManager;

//
//	UI对象创建信息
//
struct UIOBJ_CREATE_INFO 
{
	String      m_strXmlName;        // 在xml中的配置名称
    GUID        m_guid;              // 对象clsid
	pfnUICreateInstancePtr  m_func;  // 指向ObjCreator<T>::UICreateInstance
    String      m_strCategory;       // 如uictrl/listctrl，指针多级。用于在UIEditor中的ToolBox中显示
    UINT        m_nObjType;
};

struct UIRENDERBASE_CREATE_INFO
{
    String  m_strName;    // 存在很多种theme类型，但对应的控件类型不一样
    int     m_nControlType;
    int     m_nRenderType;
    pfnUICreateRenderBasePtr m_func;
};

struct UITEXTRENDERBASE_CREATE_INFO
{
    String     m_strName;
    int        m_nControlType;
    int        m_nRenderType;
    pfnUICreateTextRenderBasePtr m_func;
};

struct UILAYOUT_CREATE_INFO
{
    String  m_strName;
    int     m_nLayoutType;
    GUID    m_guid;
    pfnUICreateLayoutPtr  m_func;
};

//
//  PS: 20130110 考虑到在UIEditor中会存在两份UIApplication实例，因此不能再将UIApplication作为全局对象来实现了。
//  PS: 20130317 考虑到以后的扩展和升级，不将接口做成纯虚函数，而是采用Pimpl实现
//
class UIApplication
{
public:
	UIApplication(IUIApplication* p);
	~UIApplication(void);
    void  x_Init();  // 内部初始化，避免在构造函数中调用太多东西

	// ---------- 
    void  SetSkinDirection(LPCTSTR szDir);
    bool  LoadSkin(LPCTSTR szSkinName);

	bool  LogUI(ILog* pLog);
	bool  LogUI(LPCTSTR szLogXmlPath);
	bool  GetLog(ILog** ppLog);

	void  SetDesignMode(bool b) { m_bDesignMode = b; }
	bool  IsDesignMode() { return m_bDesignMode; }
	void  SetUIEditorPtr(IUIEditor* p) { m_pUIEditor = p; }
	IUIEditor*  GetUIEditorPtr() { return m_pUIEditor; }
	
	ISkinManager*       GetSkinMgr();
	ITopWindowManager*  GetITopWindowMgr();
	TopWindowManager*   GetTopWindowMgr() { return &m_TopWindowMgr; }
	IAnimateManager*    GetAnimateMgr();
	GifTimerManager*    GetGifTimerMgr() { return m_pGifTimerMgr; }
	IWaitForHandlesMgr*  GetWaitForHandlesMgr() { return &m_WaitForHandlesMgr; }
	IMessageFilterMgr*  GetMessageFilterMgr() { return &m_MsgFilterMgr; }
	HMODULE             GetUID2DModule();
	HMODULE             GetUID3DModule();

	SkinRes*            GetActiveSkinRes();
	ImageManager*       GetActiveSkinImageMgr();
	ImageRes*           GetActiveSkinImageRes();
	CursorRes*          GetActiveSkinCursorRes();
	GifRes*             GetActiveSkinGifRes();
	FontManager*        GetActiveSkinFontMgr();
	FontRes*            GetActiveSkinFontRes();
	ColorManager*       GetActiveSkinColorMgr();
	ColorRes*           GetActiveSkinColorRes();
	StyleManager*       GetActiveSkinStyleMgr();
	StyleRes*           GetActiveSkinStyleRes();
	LayoutManager*      GetActiveSkinLayoutMgr();
	LayoutRes*          GetActiveSkinLayoutRes();
	
	HRESULT           SetTooltipsUI(IToolTipUI* pUI);
	HRESULT           UseInnerTooltipsUI(LPCTSTR szWndID);
	bool  ShowToolTip(TOOLTIPITEM* pItem);
	void  HideToolTip();

    void  RestoreRegisterUIObject();
    bool  RegisterControlTagParseFunc(LPCTSTR szTag, pfnParseControlTag func);
    bool  GetSkinTagParseFunc(LPCTSTR szTag, pfnParseSkinTag* pFunc);
    bool  GetControlTagParseFunc(LPCTSTR szTag, pfnParseControlTag* pFunc);

    bool  RegisterUIObjectCreateData(LPCTSTR szXmlName, LPCTSTR szCategory, UINT nObjType, REFGUID guid, pfnUICreateInstancePtr pfun);
	IObject*  CreateInstanceByName(LPCTSTR szXmlName);
    bool  CreateInstanceByClsid(REFCLSID clsid, void** pOut);
    void  LoadUIObjectListToToolBox();

    bool  RegisterUIRenderBaseCreateData(LPCTSTR szName, int nType, int nControlType, pfnUICreateRenderBasePtr pfunc);
    bool  CreateRenderBaseByName(LPCTSTR szName, IObject* pObject, IRenderBase** ppOut);
    bool  CreateRenderBase(int nType, IObject* pObject, IRenderBase** ppOut);
    LPCTSTR  GetRenderBaseName(int nType);
    void  EnumRenderBaseName(pfnEnumRenderBaseNameCallback callback, WPARAM wParam, LPARAM lParam);

    bool  RegisterUITextRenderBaseCreateData(LPCTSTR szName, int nType, int nControlType, pfnUICreateTextRenderBasePtr pfunc);
    bool  CreateTextRenderBaseByName(LPCTSTR szName, IObject* pObject, ITextRenderBase** ppOut);
    bool  CreateTextRenderBase(int nType, IObject* pObject, ITextRenderBase** ppOut);
    LPCTSTR  GetTextRenderBaseName(int nType);
    void  EnumTextRenderBaseName(pfnEnumTextRenderBaseNameCallback callback, WPARAM wParam, LPARAM lParam);
    
    bool  RegisterLayoutCreateData(LPCTSTR szName, int nType, pfnUICreateLayoutPtr);
    bool  CreateLayout(int nType, IObject* pObject, ILayout** ppOut);
    bool  CreateLayoutByName(LPCTSTR szName, IObject* pObject, bool bCreateDefault, ILayout** ppOut);   
    void  EnumLayoutType(pfnEnumLayoutTypeCallback callback, WPARAM wParam, LPARAM lParam);

	BOOL  IsDialogMessage(MSG* pMsg);
	void  MsgHandleLoop(bool* pbQuitLoopRef);
	void  RedrawTopWindows() { m_TopWindowMgr.UpdateAllWindow(); }
	bool  IsUnderXpOS();
	bool  IsVistaOrWin7etc();
	HWND  GetForwardPostMessageWnd();

    // gpu 
    bool  IsGpuCompositeEnable();
    bool  EnableGpuComposite();

public:
	bool  GetUICreateInstanceFuncPtr(LPCTSTR szXmlName, pfnUICreateInstancePtr* pOut);
    bool  GetUICreateInstanceFuncPtr(REFCLSID guid, pfnUICreateInstancePtr* pOut);

    IUIApplication*  GetIUIApplication() { return m_pUIApplication; }

private:
    void  RegisterWndClass();
    HRESULT  InitLog(LPCTSTR szLogConfigFilePath);
    void  ReleaseUID2D();
	void  ReleaseUID3D();
    void  RegisterDefaultUIObject();
    void  ClearRegisterUIObject();

public:
    static ILog*     s_pLog;         // 所有的app共享一个log. log必须作成全局对象，否则不好实现在任意代码中写log
    static long      s_lUiLogCookie; // UI模块日志的标识
    static long      s_lAppCount;    // 当appcount减为0时，释放log

    static HMODULE   s_hUID2D;        // ui3d.dll
	static HMODULE   s_hUID3D;
	

private:
    typedef vector<UIOBJ_CREATE_INFO*>               UIOBJ_CREATE_DATA;
    typedef vector<UIRENDERBASE_CREATE_INFO*>        UIRENDERBASE_CREATE_DATA;
    typedef vector<UITEXTRENDERBASE_CREATE_INFO*>    UITEXTRENDERBASE_CREATE_DATA;
    typedef vector<UILAYOUT_CREATE_INFO*>            UILAYOUT_CREATE_DATA;
    typedef map<String, pfnParseSkinTag>             UISKINTAGPARSE_DATA;
    typedef map<String, pfnParseControlTag>          UICONTROLTAGPARSE_DATA;

    IUIApplication*     m_pUIApplication;              // 对外提供的接口
    OSVERSIONINFOEX     m_osvi;                        // 操作系统版本
	bool                m_bDesignMode;                 // 是否是设计模式
	IUIEditor*          m_pUIEditor;                   // 外部的编辑器指针，用于消息通知和数据获取

    list<IMessage*>           m_aliveUIObject;         // 所有UI对象列表，用于在发送消息前判断一个指针对象是否有效
	UIOBJ_CREATE_DATA         m_vecUICreateData;       // 保存UI对象的XML字符串，用于从字符串创建UI对象
    UIRENDERBASE_CREATE_DATA  m_vecUIRenderBaseCreateData;  // 用于创建RenderBase
    UITEXTRENDERBASE_CREATE_DATA  m_vecUITextRenderBaseCreateData;  // 用于从字符串创建TextRenderBase
    UILAYOUT_CREATE_DATA      m_vecUILayoutCreateData;
    UISKINTAGPARSE_DATA       m_mapSkinTagParseData;
    UICONTROLTAGPARSE_DATA    m_mapControlTagParseData;

    TopWindowManager    m_TopWindowMgr; 
    SkinManager         m_skinMgr;       // 需要提前释放，做成指针模式
    AnimateManager*     m_pAnimateMgr;   // 需要提前释放，做成指针模式
	GifTimerManager*    m_pGifTimerMgr;

    WaitForHandlesMgr   m_WaitForHandlesMgr;
    ForwardPostMessageWindow  m_WndForwardPostMsg; 
    MessageFilterMgr    m_MsgFilterMgr;
    ToolTipManager      m_ToolTipMgr;

    // gpu合成
    bool  m_bGpuEnable;
};

}

#endif  // UIAPPLICATION_H_369FA8A6_A4A8_47a0_9E84_082B3AABA2A1