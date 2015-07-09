#ifndef CUSTOMWINDOW_H_E436450D_B466_4889_A8AA_CA6FCABCEB24
#define CUSTOMWINDOW_H_E436450D_B466_4889_A8AA_CA6FCABCEB24

#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
#include "UISDK\Kernel\Src\UIObject\Window\window.h"
#include "UISDK\Kernel\Inc\Interface\iwndtransmode.h"

/*
 �ǿͻ����ػ��һЩ������

. WM_NCACTIVATE:
  ��������������Ϣ��aero transparent���ڼ���ʱ���Ὣ��Ӱ�޸�Ϊ����Ӱ
  ����ſ������Ϣ���ھ����������棬����ʧ����ˢ��������

. WM_NCCALCSIZE
  �����޸Ŀͻ���λ�á�ֱ��return TRUE;�������ε��ǿͻ���

. WM_NCHITTEST
  �����WM_NCHITTEST�������HTCLOSE,HTMAXBUTTON,HTMINBUTTON��ϵͳ��ǿ��ˢ����������ť����Ҫ���ε�

. WM_NCPAINT
  ������θ���Ϣ����aero trans��������ʾ��Ӱ�ˡ�
  ����ſ������Ϣ���ھ����������洰�ڵõ�����ʱ��ˢ����������Ŀǰֻ��antiģʽ�·����и����⣬mask color/mask alpha���д���֤��

. ����ģʽ�����WM_SETTEXT,WM_SETICON (WM_NCLBUTTONDOWN??)
  ���ø�api��ϵͳ��ǿ��ˢһ�±���������ν��?
  http://blog.sina.com.cn/s/blog_437fd95e01013lrk.html
  Chrome: ui\views\widget\native_widget_win.cc  void NativeWidgetWin::LockUpdates();
  ������WS_VISIBLE����ȥ����Ȼ���ټӻ���

  ���⻹�У�WM_NCLBUTTONDOWN��EnableMenuItem(called from our WM_INITMENU handler)

. ����û���ص���Ϣ�� ��Ŀǰ���Ի�û�������ǵ�����֮����
  WM_NCUAHDRAWCAPTION
  WM_NCUAHDRAWFRAME

*/


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

	// ��������
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

	// ��Ϣ��Ӧ
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
	// ����͸�������������
	//
    IWindowTransparent*  m_pWindowTransparent;
	bool  m_bNeedToSetWindowRgn;   // �Ƿ���Ҫ�������ô�����������
    bool  m_bFirstNcCalcSize;
    bool  m_bRemoveMAXBOX;        // ���ʱȥ��MAXBOX��ʽ
    bool  m_bAeroCaptionAnimate;  // ʹ��aero ws_caption ��������ʾ�����ء����С����С����

    //
    // �߿���ק
    //
	long  m_lResizeCapability;  // ���ڱ�Ե����������־λ
	long  m_lResizeBorder;      // ������ק�ı�Ե�����С
    long  m_lMaximizeBorder;    // ���ʱ��Ҫλ����Ļ��ı߿��С��������resizeborder��һ��

    friend class AeroWindowWrap;
    friend class LayeredWindowWrap;
};

}
#endif // CUSTOMWINDOW_H_E436450D_B466_4889_A8AA_CA6FCABCEB24