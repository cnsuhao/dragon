#pragma once
#include "windowlessricheditex.h"
#include "UISDK\Control\Inc\Interface\irichedit.h"

//
//	RichEdit编辑框控件
//
//  该类仅用于包装WindowlessRichEdit，为WindowlessRichEdit提供一个容器和消息转发器
//  为WindowlessRichEdit提供坐标范围、外部消息等。
//  RichEdit对应的方法和接口全部由WindowlessRichEdit实现，外部进行操作时，可直接获
//  取WindowlessRichEdit的指针进行调用。
//
namespace UI
{
interface IScrollBarManager;
interface ISystemVScrollBar;

class  RichEdit : public MessageProxy
{
public:
	RichEdit();
    ~RichEdit();
    UI_DECLARE_OBJECT(RichEdit, OBJ_CONTROL|CONTROL_RICHEDIT);

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT2( OnPaint )
		UIMSG_WM_ERASEBKGND( OnEraseBkgnd )
		UIMESSAGE_HANDLER_EX( UI_WM_REDRAWOBJECT, OnRedrawObject )

		UIMESSAGE_HANDLER_EX( WM_SETCURSOR, OnForwardMessage )
		UIMESSAGE_HANDLER_EX( WM_CHAR, OnForwardMessage )
		UIMESSAGE_HANDLER_EX( WM_KEYDOWN, OnForwardMessage )
		UIMESSAGE_HANDLER_EX( WM_KEYUP, OnForwardMessage )
		UIMESSAGE_HANDLER_EX( WM_WINDOWPOSCHANGED, OnForwardMessage )
		UIMESSAGE_HANDLER_EX( WM_TIMER, OnForwardMessage )
		UIMESSAGE_HANDLER_EX( UI_WM_GIFFRAME_TICK, OnForwardMessage)
		UIMSG_WM_VISIBLE_CHANGED( OnVisibleChanged )
		UIMSG_WM_SHOWWINDOW( OnShowWindow )
		UIMSG_WM_SETFOCUS( OnSetFocus )
		UIMSG_WM_KILLFOCUS( OnKillFocus )
		UIMSG_WM_SIZE( OnSize ) 

		// 在Scrollmgr处理之前先拦截UI_WM_GETSCROLLOFFSET
		// 因为richedit内部有自己的偏移信息，而不能使用UI控件的
		// 偏移信息，否则就会导致光标，内容显示错误。

		UIMESSAGE_HANDLER_EX( WM_VSCROLL, OnScroll )
		UIMESSAGE_HANDLER_EX( WM_HSCROLL, OnScroll )
		UIMESSAGE_HANDLER_EX( UI_WM_GETSCROLLOFFSET,OnGetScrollOffset )
        UIMESSAGE_HANDLER_EX( WM_MOUSEWHEEL, OnForwardMessage )
		UIMSG_WM_GETDLGCODE( OnGetDlgCode )
        UIMESSAGE_HANDLER_EX( WM_IME_STARTCOMPOSITION, OnForwardMessage)
        UIMESSAGE_HANDLER_EX( WM_IME_ENDCOMPOSITION, OnForwardMessage )
        UIMESSAGE_HANDLER_EX( WM_IME_COMPOSITION, OnForwardMessage )
        UIMESSAGE_HANDLER_EX( WM_IME_REQUEST, OnForwardMessage )
        UIMESSAGE_HANDLER_EX( WM_IME_CHAR,  OnForwardMessage )

	//	UICHAIN_MSG_MAP_POINT_MEMBER(m_pMgrScrollbar)  
		UIMSG_WM_RBUTTONDOWN( OnRButtonDown )
		UIMESSAGE_RANGE_HANDLER_EX( WM_MOUSEFIRST,WM_MOUSELAST, OnForwardMessage )
        UIMESSAGE_HANDLER_EX( WM_MOUSELEAVE, OnForwardMessage )

        UIMESSAGE_HANDLER_EX( UI_WM_DROPTARGETEVENT, OnForwardMessage )

		UIMSG_WM_INITIALIZE( OnInitialize )
        UIMSG_WM_QUERYINTERFACE( QueryInterface )
        UIMSG_WM_GETOBJECTINFO( OnGetObjectInfo ) 
        UIMSG_WM_SERIALIZE( OnSerialize )
        UIMSG_WM_FINALCONSTRUCT( FinalConstruct )
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(RichEdit, IControl)

    void  SetIRichEdit(IRichEdit* p);
    IRichEdit*  GetIRichEdit();

public:
	WindowlessRichEdit& GetRichEdit();
	IScrollBarManager*  GetScrollMgr();
    CCaret*  GetCaret();
    void  SetReadOnly(bool fReadOnly);
    void  EnableBubbleMode(bool b);
    bool  IsBubbleMode();

    void  InvalidateRect(RECT* prcWnd);
    void  InvalidateRect2(RECT* prcWndArray, int nCount);
    HRESULT  OnTxNotify(DWORD iNotify, void* pv, BOOL& bHandled);

    void  RegisterDropEvent();
    void  RevokeDropEvent();

protected:
    HRESULT  FinalConstruct(IUIApplication* p);
    void  OnSerialize(SERIALIZEDATA* pData);

	void  OnInitialize();
	void  OnEraseBkgnd(IRenderTarget* pRenderTarget);
	void  OnPaint(IRenderTarget* pRenderTarget, RenderContext* pContext);
	LRESULT  OnRedrawObject(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnForwardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT  OnGetScrollOffset(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT  OnScroll(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	void  OnSetFocus(IObject* pOldFocusObj);
	void  OnKillFocus(IObject* pNewFocusObj);
	void  OnSize(UINT nType, int cx, int cy);
	void  OnRButtonDown(UINT nFlags, CPoint point);
	UINT  OnGetDlgCode(LPMSG lpMsg);
	
	void  OnShowWindow(BOOL bShow, UINT nStatus);
	void  OnVisibleChanged(BOOL bVisible, IObject* pParent);

    long  OnRequestAutoSize(RECT* prcRequest);
    void  OnEnChanged(BOOL& bHandled);

protected:
    IRichEdit*  m_pIRichEdit;

	WindowlessRichEditEx   m_wrapRichEidt;
	IScrollBarManager*   m_pMgrScrollBar;
    ISystemVScrollBar*   m_pVScrollBar;

    CCaret   m_caret;         // 光标
	bool   m_bRedrawing;      // 用于区别是外部直接调用的UpdateObject，还是OnRedrawObject调用的UpdateObject
    bool   m_bPostRedrawMsg;  // 标记当前已经post了一个redraw消息，并且还没有响应。在此期间不再post第二个redraw消息
    CRect  m_rcInvalidate;    // TxInvalidateRect辅助变量。无效区域的集合

    bool  m_bByteLimit;

    // 自适应大小编辑框
    bool  m_bAutoSize;
    long  m_lSingleLineMaxWidth;
    long  m_lSingleLineMinWidth;
    long  m_lMultiLineMaxHeight;
    long  m_lMultiLineMinHeight;

    // 气泡模式
    bool  m_bBubbleMode;
};

}