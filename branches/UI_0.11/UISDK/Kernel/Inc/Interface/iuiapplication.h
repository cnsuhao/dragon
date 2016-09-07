#ifndef IUIAPPLICATION_H_61E30001_8C5F_4e34_907B_5218915BC4C2
#define IUIAPPLICATION_H_61E30001_8C5F_4e34_907B_5218915BC4C2
interface ILog;

namespace UI
{
class UIApplication;
interface ILayout;
interface IUIEditor;
interface ISkinRes;      
interface IImageManager;
interface IImageRes;   
interface ICursorRes;
interface IGifRes;
interface IFontManager;
interface IFontRes;
interface IColorManager;
interface IColorRes;
interface IStyleManager;
interface IStyleRes;
interface ILayoutManager;
interface ILayoutRes;

interface UISDKAPI IUIApplication : IUnknown
{
    IUIApplication();
    virtual ~IUIApplication();
    UIApplication*  GetImpl();

    ISkinRes*  LoadSkinRes(LPCTSTR szSkinName);

	bool  LogUI(ILog* pLog);
	bool  LogUI(LPCTSTR szLogXmlPath);
    bool  GetLog(ILog** ppLog);
    HMODULE  GetUID3DModule();
	HMODULE  GetUID2DModule();

    void  SetDesignMode(bool b);
	void  SetUIEditorPtr(IUIEditor*);
    bool  IsDesignMode();
    IUIEditor*  GetUIEditorPtr();

    ISkinManager*       GetSkinMgr();
    ITopWindowManager*  GetTopWindowMgr();
    IAnimateManager*    GetAnimateMgr();
    IWaitForHandlesMgr*  GetWaitForHandlesMgr();
    IMessageFilterMgr*  GetMessageFilterMgr();

    ISkinRes*       GetDefaultSkinRes();
    IImageManager*  GetActiveSkinImageMgr();
    IImageRes*      GetActiveSkinImageRes();
    ICursorRes*     GetActiveSkinCursorRes();
    IGifRes*        GetActiveSkinGifRes();
    IFontManager*   GetActiveSkinFontMgr();
    IFontRes*       GetActiveSkinFontRes();
    IColorManager*  GetActiveSkinColorMgr();
    IColorRes*      GetActiveSkinColorRes();
    IStyleManager*  GetActiveSkinStyleMgr();
    IStyleRes*      GetActiveSkinStyleRes();
    ILayoutManager* GetActiveSkinLayoutMgr();
    ILayoutRes*     GetActiveSkinLayoutRes();

    void  RestoreRegisterUIObject();
    bool  RegisterControlTagParseFunc(LPCTSTR szTag, pfnParseControlTag func);
    bool  GetSkinTagParseFunc(LPCTSTR szTag, pfnParseSkinTag* pFunc);
    bool  GetControlTagParseFunc(LPCTSTR szTag, pfnParseControlTag* pFunc);
    
    IObject*  CreateInstanceByName(LPCTSTR szName, ISkinRes* pISkinRes);
    IObject*  CreateInstanceByClsid(REFCLSID clsid, ISkinRes* pISkinRes);
    bool  RegisterUIObjectCreateData(LPCTSTR szName, LPCTSTR szCategory, UINT nObjType, REFCLSID guid, pfnUICreateInstancePtr pfunc);
    void  LoadUIObjectListToToolBox();

    bool  RegisterUIRenderBaseCreateData(LPCTSTR szName, int nType, pfnUICreateRenderBasePtr pfunc);
    bool  CreateRenderBaseByName(LPCTSTR szName, IObject* pObject, IRenderBase** ppOut);
    bool  CreateRenderBase(int nType, IObject* pObject, IRenderBase** ppOut);
    LPCTSTR  GetRenderBaseName(int nType);
    void  EnumRenderBaseName(pfnEnumRenderBaseNameCallback callback, WPARAM wParam, LPARAM lParam);

    bool  RegisterUITextRenderBaseCreateData(LPCTSTR szName, int nType, pfnUICreateTextRenderBasePtr pfunc);
    bool  CreateTextRenderBaseByName(LPCTSTR szName, IObject* pObject, ITextRenderBase** ppOut);
    bool  CreateTextRenderBase(int nType, IObject* pObject, ITextRenderBase** ppOut);
    void  EnumTextRenderBaseName(pfnEnumTextRenderBaseNameCallback callback, WPARAM wParam, LPARAM lParam);

    bool  RegisterLayoutCreateData(LPCTSTR szName, int nType, pfnUICreateLayoutPtr);
    bool  CreateLayoutByName(LPCTSTR szName, IObject* pObject, bool bCreateDefault, ILayout** ppOut);
    bool  CreateLayout(int nType, IObject* pObject, ILayout** ppOut);
    void  EnumLayoutType(pfnEnumLayoutTypeCallback callback, WPARAM wParam, LPARAM lParam);

    BOOL  IsDialogMessage(MSG* pMsg);
    void  MsgHandleLoop(bool* pbQuitLoopRef=NULL);
    void  RedrawTopWindows();

    bool  ShowToolTip(TOOLTIPITEM* pItem);
    void  HideToolTip();
    UINT_PTR  SetTimer(int nElapse, IMessage* pNotify);
    UINT_PTR  SetTimerById(int nElapse, int nId, IMessage* pNotify);
    UINT_PTR  SetTimer(int nElapse, TimerItem* pTimerItem);
    void  KillTimer(UINT_PTR& nTimerID);
    void  KillTimerById(int nId, IMessage* pNotify);
    void  KillTimerByNotify(IMessage* pNotify);

    HDC   GetCacheDC();
    void  ReleaseCacheDC(HDC hDC);
    HBITMAP  GetCacheBitmap(int nWidth, int nHeight);  // 注：不要释放该HBITMAP，由内部维护
    void  ShadowBlur(HBITMAP hBitmap, COLORREF colorShadow, RECT* prcBlur, int nRadius);

    bool  IsUnderXpOS();
    bool  IsVistaOrWin7etc();
    bool  IsAeroEnable();
	bool  IsGpuCompositeEnable();
    bool  EnableGpuComposite();

    HWND  GetForwardPostMessageWnd();
    IWindowBase*  GetWindowBaseFromHWND(HWND hWnd);

private:
    UIApplication*  m_pImpl;
};

interface UISDKAPI UISDKVersion
{
	static int  GetMajor();
	static int  GetMinor();
	static int  GetPatch();
	static int  GetVersionText(TCHAR* szText, int nTextSize);
};

}

#endif