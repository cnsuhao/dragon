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
//	UI���󴴽���Ϣ
//
struct UIOBJ_CREATE_INFO 
{
	String      m_strXmlName;        // ��xml�е���������
    GUID        m_guid;              // ����clsid
	pfnUICreateInstancePtr  m_func;  // ָ��ObjCreator<T>::UICreateInstance
    String      m_strCategory;       // ��uictrl/listctrl��ָ��༶��������UIEditor�е�ToolBox����ʾ
    UINT        m_nObjType;
};

struct UIRENDERBASE_CREATE_INFO
{
    String  m_strName;    // ���ںܶ���theme���ͣ�����Ӧ�Ŀؼ����Ͳ�һ��
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
//  PS: 20130110 ���ǵ���UIEditor�л��������UIApplicationʵ������˲����ٽ�UIApplication��Ϊȫ�ֶ�����ʵ���ˡ�
//  PS: 20130317 ���ǵ��Ժ����չ�������������ӿ����ɴ��麯�������ǲ���Pimplʵ��
//
class UIApplication
{
public:
	UIApplication(IUIApplication* p);
	~UIApplication(void);
    void  x_Init();  // �ڲ���ʼ���������ڹ��캯���е���̫�ණ��

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
    static ILog*     s_pLog;         // ���е�app����һ��log. log��������ȫ�ֶ��󣬷��򲻺�ʵ�������������дlog
    static long      s_lUiLogCookie; // UIģ����־�ı�ʶ
    static long      s_lAppCount;    // ��appcount��Ϊ0ʱ���ͷ�log

    static HMODULE   s_hUID2D;        // ui3d.dll
	static HMODULE   s_hUID3D;
	

private:
    typedef vector<UIOBJ_CREATE_INFO*>               UIOBJ_CREATE_DATA;
    typedef vector<UIRENDERBASE_CREATE_INFO*>        UIRENDERBASE_CREATE_DATA;
    typedef vector<UITEXTRENDERBASE_CREATE_INFO*>    UITEXTRENDERBASE_CREATE_DATA;
    typedef vector<UILAYOUT_CREATE_INFO*>            UILAYOUT_CREATE_DATA;
    typedef map<String, pfnParseSkinTag>             UISKINTAGPARSE_DATA;
    typedef map<String, pfnParseControlTag>          UICONTROLTAGPARSE_DATA;

    IUIApplication*     m_pUIApplication;              // �����ṩ�Ľӿ�
    OSVERSIONINFOEX     m_osvi;                        // ����ϵͳ�汾
	bool                m_bDesignMode;                 // �Ƿ������ģʽ
	IUIEditor*          m_pUIEditor;                   // �ⲿ�ı༭��ָ�룬������Ϣ֪ͨ�����ݻ�ȡ

    list<IMessage*>           m_aliveUIObject;         // ����UI�����б������ڷ�����Ϣǰ�ж�һ��ָ������Ƿ���Ч
	UIOBJ_CREATE_DATA         m_vecUICreateData;       // ����UI�����XML�ַ��������ڴ��ַ�������UI����
    UIRENDERBASE_CREATE_DATA  m_vecUIRenderBaseCreateData;  // ���ڴ���RenderBase
    UITEXTRENDERBASE_CREATE_DATA  m_vecUITextRenderBaseCreateData;  // ���ڴ��ַ�������TextRenderBase
    UILAYOUT_CREATE_DATA      m_vecUILayoutCreateData;
    UISKINTAGPARSE_DATA       m_mapSkinTagParseData;
    UICONTROLTAGPARSE_DATA    m_mapControlTagParseData;

    TopWindowManager    m_TopWindowMgr; 
    SkinManager         m_skinMgr;       // ��Ҫ��ǰ�ͷţ�����ָ��ģʽ
    AnimateManager*     m_pAnimateMgr;   // ��Ҫ��ǰ�ͷţ�����ָ��ģʽ
	GifTimerManager*    m_pGifTimerMgr;

    WaitForHandlesMgr   m_WaitForHandlesMgr;
    ForwardPostMessageWindow  m_WndForwardPostMsg; 
    MessageFilterMgr    m_MsgFilterMgr;
    ToolTipManager      m_ToolTipMgr;

    // gpu�ϳ�
    bool  m_bGpuEnable;
};

}

#endif  // UIAPPLICATION_H_369FA8A6_A4A8_47a0_9E84_082B3AABA2A1