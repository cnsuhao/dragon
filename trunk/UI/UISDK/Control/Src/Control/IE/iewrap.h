#pragma once
#include "UISDK\Control\Inc\Interface\iiewrap.h"
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
#include "iesite.h"

// http://www.cnblogs.com/lucc/archive/2010/11/24/1886087.html  WebBrowser介绍——Javascript与C++互操作 
// CDHtmlDialog  mfc
namespace UI
{
class PopupIEWindow;

class IEWrap : public MessageProxy, public IDWebBrowserEvent2
{
public:
    IEWrap();
    ~IEWrap();

	UI_DECLARE_OBJECT3(IEWrap, OBJ_HWNDHOST|CONTROL_IE, _T("UICtrl/Control"))

	UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT2(OnPaint)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(IEWrap, IHwndHost)

    void  SetIIEWrap(IIEWrap* p) 
    { m_pIIEWrap = p; SetIMessageProxy(static_cast<IMessage*>(p)); }
    IIEWrap*  GetIIEWrap() { return m_pIIEWrap; }

public:
    void OnPaint(IRenderTarget* pRenderTarget, RenderContext* pContext);
	void  SetAttribute(IMapAttribute* pMatAttrib, bool bReload);
    IWebBrowser2*  GetWebBrowser2() 
    { return m_pWebBrowser; }

	HRESULT  InvokeJsFunction(BSTR bstrFuncName);
	HRESULT  Navigate(const TCHAR* szUrl);

public:
    virtual void DocumentComplete(IDispatch *pDisp, VARIANT *URL);

protected:
	void      CreateControl();
	HRESULT   SetIESize(int cx, int cy);
	HRESULT   SetIEPos(LPRECT lprc);

    bool  UnSubclassIEWindow();
    bool  SubclassIEWindow();
    static LRESULT CALLBACK  _WndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    LRESULT  WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
    LRESULT  DefWindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

    void  Render2DC(HDC hDC, int nxOffset, int nyOffset, CRect* prcUpdate);

protected:
    IIEWrap*   m_pIIEWrap;
    bool  m_bNeedFixAlpha;
    bool  m_bPrintingWindow;

	IEEmbeddingSite*  m_pSite;
	IWebBrowser2*     m_pWebBrowser;
    DWORD  m_dwEventCookie;

	HWND   m_hWndShellEmbedding;    // 三层窗口句柄
    HWND   m_hWndShellDocObjectView;
    HWND   m_hWndIE;

	IOleObject*       m_pOleObject;
	IViewObject2*     m_pViewObject;

    CWndProcThunk  m_thunk;   // ATL中的THUNK，用于将一个窗口过程作成自己的成员函数
    WNDPROC  m_oldWndProc;    // 该窗口的原始窗口过程
};

}