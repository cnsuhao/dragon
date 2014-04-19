#ifndef CUSTOMWINDOW_H_E436450D_B466_4889_A8AA_CA6FCABCEB24
#define CUSTOMWINDOW_H_E436450D_B466_4889_A8AA_CA6FCABCEB24

#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
#include "UISDK\Kernel\Src\UIObject\Window\window.h"
#include "UISDK\Kernel\Inc\Interface\iwndtransmode.h"


namespace UI
{

class LayeredWindowWrap;
class AeroWindowWrap;

//
// �Զ��崰���࣬�޷ǿͻ�����
//
class CustomWindow : public Window
{
public:
	CustomWindow();
	~CustomWindow(void);

	UI_DECLARE_OBJECT3(CustomWindow, OBJ_WINDOW, _T("Kernel/Window"))

	VIRTUAL_BEGIN_MSG_MAP(CustomWindow)
        CHAIN_MSG_MAP_MEMBER_P(m_pTransparentMode)
		MESSAGE_HANDLER(WM_NCPAINT, _OnNcPaint)
		MESSAGE_HANDLER(WM_NCACTIVATE, _OnNcActivate)
		MESSAGE_HANDLER(WM_NCDESTROY,  _OnNcDestroy)
        MESSAGE_HANDLER(WM_SIZE, _OnSize )
        MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, _OnWindowPosChanging)
		CHAIN_MSG_MAP(Window)
	END_MSG_MAP()

	UI_BEGIN_MSG_MAP
        UICHAIN_MSG_MAP_POINT_MEMBER(m_pTransparentMode)
		UIMSG_WM_SETCURSOR(OnSetCursor)
		UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
		UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
		UIMSG_WM_HITTEST(OnHitTest)
		
//		UIMSG_WM_NCHITTEST(OnNcHitTest)
// 		UIMSG_WM_NCMOUSEMOVE    (OnNcMouseMove)
// 		UIMSG_WM_NCMOUSEHOVER   (OnNcMouseHover)
// 		UIMSG_WM_NCMOUSELEAVE   (OnNcMouseLeave)
// 		UIMSG_WM_NCLBUTTONDOWN  (OnNcLButtonDown)
// 		UIMSG_WM_NCLBUTTONUP    (OnNcLButtonUp)
// 		UIMSG_WM_NCLBUTTONDBLCLK(OnNcLButtonDblClk)
// 		UIMSG_WM_NCRBUTTONDOWN  (OnNcRButtonDown)
// 		UIMSG_WM_NCRBUTTONUP    (OnNcRButtonUp)
// 		UIMSG_WM_NCRBUTTONDBLCLK(OnNcRButtonDblClk)
// 		UIMSG_WM_NCMBUTTONDOWN  (OnNcMButtonDown)
// 		UIMSG_WM_NCMBUTTONUP    (OnNcMButtonUp)
// 		UIMSG_WM_NCMBUTTONDBLCLK(OnNcMButtonDblClk)

        UIMSG_WM_GET_WINDOW_TRANSPARENT_MODE(GetWndTransMode)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_EDITORGETATTRLIST(OnEditorGetAttrList)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_PRECREATEWINDOW(PreCreateWindow)
	UI_END_MSG_MAP_CHAIN_PARENT(Window)

    ICustomWindow* GetICustomWindow() { return m_pICustomWindow; }
    void SetICustomWindow(ICustomWindow* p) { m_pICustomWindow = p; }

	// ��������
public:
	void  SetResizeCapability(UINT nType);
	bool  IsWindowLayered();
	void  EnableWindowLayered(bool b);
    void  EnableWindowAero(bool b);
	void  SetWindowMaskAlpha();
	void  SetWindowMaskColor();

	void  UpdateWindowRgn();
	void  SetWndTransMode(IWndTransMode* pMode);
	void  SetWndTransMode(WINDOW_TRANSPARENT_MODE eMode, bool bRedraw);
    WINDOW_TRANSPARENT_MODE  GetWndTransMode();

protected:
    void  ResetAttribute();
    void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload=false);
    void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData);

	BOOL  PreCreateWindow(CREATESTRUCT* pcs);

    virtual void  OnInnerInitWindow();
	virtual void  CommitDoubleBuffet2Window(HDC hDCWnd, RECT* prcCommit, int nRectCount=1);
	virtual void  virtualOnPostDrawObjectErasebkgnd();

	// ��Ϣ��Ӧ
protected:
	LRESULT  _OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT  _OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT  _OnNcDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  _OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  _OnWindowPosChanging( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );

	void  OnEraseBkgnd(IRenderTarget* hDC);
	BOOL  OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
	void  OnLButtonDown(UINT nFlags, POINT point);
    UINT  OnHitTest(POINT* pt, POINT*  ptInChild);

//	int  OnCreate(LPCREATESTRUCT lpCreateStruct);
//	LRESULT  OnNcHitTest( POINT pt );
// 	void OnNcMouseMove( UINT nHitTest, POINT point );
// 	void OnNcMouseHover( UINT nHitTest, POINT point ); 
// 	void OnNcMouseLeave();
// 	void OnNcLButtonDown( UINT nHitTest, POINT point );
// 	void OnNcLButtonUp( UINT nHitTest, POINT point );
// 	void OnNcLButtonDblClk( UINT nHitTest, POINT point );
// 	void OnNcRButtonDown( UINT nHitTest, POINT point );
// 	void OnNcRButtonUp( UINT nHitTest, POINT point );
// 	void OnNcRButtonDblClk( UINT nHitTest, POINT point );
// 	void OnNcMButtonDown( UINT nHitTest, POINT point );
// 	void OnNcMButtonUp( UINT nHitTest, POINT point );
// 	void OnNcMButtonDblClk( UINT nHitTest, POINT point );

protected:
	bool    TestResizeBit(int nBit);

protected:
    ICustomWindow*  m_pICustomWindow;
    
    //
	// ����͸�������������
	//
    IWndTransMode*  m_pTransparentMode;
	bool  m_bNeedToSetWindowRgn;   // �Ƿ���Ҫ�������ô�����������

    //
    // �߿���ק
    //
	int   m_nResizeCapability;  // ���ڱ�Ե����������־λ
	int   m_nResizeBorder;      // ������ק�ı�Ե�����С
    int   m_nMaximizeBorder;    // ���ʱ��Ҫλ����Ļ��ı߿��С��������resizeborder��һ��

    friend class AeroWindowWrap;
    friend class LayeredWindowWrap;
};

}
#endif // CUSTOMWINDOW_H_E436450D_B466_4889_A8AA_CA6FCABCEB24