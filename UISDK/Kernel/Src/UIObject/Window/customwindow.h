#ifndef CUSTOMWINDOW_H_E436450D_B466_4889_A8AA_CA6FCABCEB24
#define CUSTOMWINDOW_H_E436450D_B466_4889_A8AA_CA6FCABCEB24

#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
#include "UISDK\Kernel\Src\UIObject\Window\window.h"
#include "UISDK\Kernel\Inc\Interface\iwndtransmode.h"

/*
 非客户区重绘的一些东西：

. WM_NCACTIVATE:
  如果拦截了这个消息，aero transparent将在激活时不会将阴影修改为厚阴影
  如果放开这个消息，在经典主题下面，窗口失焦会刷出标题栏

. WM_NCCALCSIZE
  用于修改客户区位置。直接return TRUE;即可屏蔽掉非客户区

. WM_NCHITTEST
  鼠标在WM_NCHITTEST如果返回HTCLOSE,HTMAXBUTTON,HTMINBUTTON，系统会强制刷出这三个按钮，需要屏蔽掉

. WM_NCPAINT
  如果屏蔽该消息，那aero trans不会再显示阴影了。
  如果放开这个消息，在经典主题下面窗口得到激活时将刷出标题栏（目前只有anti模式下发现有该问题，mask color/mask alpha还有待验证）

. 经典模式下面的WM_SETTEXT,WM_SETICON (WM_NCLBUTTONDOWN??)
  调用该api，系统会强制刷一下标题样，如何解决?
  http://blog.sina.com.cn/s/blog_437fd95e01013lrk.html
  Chrome: ui\views\widget\native_widget_win.cc  void NativeWidgetWin::LockUpdates();
  将窗口WS_VISIBLE属性去掉，然后再加回来

  另外还有：WM_NCLBUTTONDOWN、EnableMenuItem(called from our WM_INITMENU handler)

. 两个没记载的消息： （目前测试还没发现它们的神奇之处）
  WM_NCUAHDRAWCAPTION
  WM_NCUAHDRAWFRAME

*/


namespace UI
{

class LayeredWindowWrap;
class AeroWindowWrap;

//
// 自定义窗口类，无非客户区域
//
class CustomWindow : public Window
{
public:
	CustomWindow();
	~CustomWindow(void);

	UI_DECLARE_OBJECT(CustomWindow, OBJ_WINDOW)

	VIRTUAL_BEGIN_MSG_MAP(CustomWindow)
        MESSAGE_HANDLER(WM_NCCALCSIZE, _OnNcCalcSize)
        CHAIN_MSG_MAP_MEMBER_P(m_pWindowTransparent)
//		MESSAGE_HANDLER(WM_NCPAINT, _OnNcPaint)
        MESSAGE_HANDLER(WM_NCHITTEST, _OnNcHitTest)
		MESSAGE_HANDLER(WM_NCACTIVATE, _OnNcActivate)
        MESSAGE_HANDLER(WM_SETTEXT, _OnSetTextIcon)
        MESSAGE_HANDLER(WM_SETICON, _OnSetTextIcon)
        MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, _OnWindowPosChanging)
        MESSAGE_HANDLER(WM_GETMINMAXINFO, _OnGetMinMaxInfo)
        MESSAGE_HANDLER(WM_NCUAHDRAWCAPTION, _OnNCUAHDrawCaption)
        MESSAGE_HANDLER(WM_NCUAHDRAWFRAME, _OnNCUAHDrawFrame)
#if(_WIN32_WINNT >= 0x0600)
        MESSAGE_HANDLER(WM_DWMCOMPOSITIONCHANGED, _OnDwmCompositionChanged)
#endif
		CHAIN_MSG_MAP(Window)
	END_MSG_MAP()

	UI_BEGIN_MSG_MAP
        UICHAIN_MSG_MAP_POINT_MEMBER(m_pWindowTransparent)
		UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
		UIMSG_WM_HITTEST(OnHitTest)
        UIMSG_WM_SYSCOMMAND(OnSysCommand)
		
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

        UIMSG_WM_GET_WINDOW_TRANSPARENT_MODE(GetWindowTransparentType)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_SERIALIZE(OnSerialize)
        UIMSG_WM_PRECREATEWINDOW(PreCreateWindow)
	UI_END_MSG_MAP_CHAIN_PARENT(Window)

    ICustomWindow* GetICustomWindow() { return m_pICustomWindow; }
    void SetICustomWindow(ICustomWindow* p) { m_pICustomWindow = p; }

	// 公共方法
public:
	void  SetResizeCapability(long nType);
    long  GetResizeCapability();
	void  UpdateWindowRgn();
	
    bool  IsWindowLayered();
    void  EnableWindowLayered(bool b);
    void  EnableWindowAero(bool b);
    void  SetWindowTransparentType(WINDOW_TRANSPARENT_TYPE l);
    void  SetWindowTransparent(IWindowTransparent* pMode);
    WINDOW_TRANSPARENT_TYPE  GetWindowTransparentType();
    IWindowTransparent*  GetWindowTransparentPtr();

protected:
	void  OnSerialize(SERIALIZEDATA* pData);

private:
	BOOL  PreCreateWindow(CREATESTRUCT* pcs);

    virtual void  OnInnerInitWindow();
	virtual void  CommitDoubleBuffet2Window(HDC hDCWnd, RECT* prcCommit, int nRectCount=1);
	virtual void  virtualOnPostDrawObjectErasebkgnd();

    bool  IsBorderlessWindow();

	// 消息响应
protected:
    
    LRESULT  _OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  _OnWindowPosChanging( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
    LRESULT  _OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  _OnGetMinMaxInfo( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
//	LRESULT  _OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT  _OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  _OnSetTextIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  _OnNCUAHDrawCaption(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  _OnNCUAHDrawFrame(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  _OnDwmCompositionChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    
	void  OnEraseBkgnd(IRenderTarget* hDC);
    int   OnHitTest(POINT* pt, POINT*  ptInChild);
    void  OnSysCommand(UINT nID, CPoint point);

    virtual void  virtualOnSize(UINT nType, UINT nWidth, UINT nHeight);

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
	// 窗口透明处理相关数据
	//
    IWindowTransparent*  m_pWindowTransparent;
	bool  m_bNeedToSetWindowRgn;   // 是否需要重新设置窗口异形区域
    bool  m_bFirstNcCalcSize;
    bool  m_bRemoveMAXBOX;        // 最大化时去掉MAXBOX样式
    bool  m_bAeroCaptionAnimate;  // 使用aero ws_caption 动画（显示、隐藏、最大小、最小化）

    //
    // 边框拖拽
    //
	long  m_lResizeCapability;  // 窗口边缘伸缩能力标志位
	long  m_lResizeBorder;      // 用于拖拽的边缘区域大小
    long  m_lMaximizeBorder;    // 最大化时需要位于屏幕外的边框大小，可以与resizeborder不一样

    friend class AeroWindowWrap;
    friend class LayeredWindowWrap;
};

}
#endif // CUSTOMWINDOW_H_E436450D_B466_4889_A8AA_CA6FCABCEB24