#pragma once
#include "Other\resource.h"
#include "..\Dialogs.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "Util\trackrect.h"
#include "Dialog\Framework\childframe.h"
#include "LayoutPropertyHandler.h"
class CLayoutCodeDlg;

class CLayoutEditorDlg : public IWindow,
                       public ILayoutMDIClientView,
                       public IRectTrackerRsp
{
public:
    enum {
        WM_USER_REFRESH = WM_USER + 0x100,
    };

    enum MOUSE_ACTION
    {
        MOUSE_ACTION_NONE,
        MOUSE_ACTION_MOVE_WINDOW_DRWA_POS = 1,
    };

	CLayoutEditorDlg();
	~CLayoutEditorDlg();

    UI_BEGIN_MSG_MAP_Ixxx(CLayoutEditorDlg)

        if (m_recttracker.ProcessMessage(uMsg, wParam, lParam, &lResult))
        {
            return TRUE;
        }

        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
        UIMSG_WM_SIZE(OnSize)

        UIMSG_WM_MOUSEMOVE(OnMouseMove)
        UIMSG_WM_SETCURSOR(OnSetCursor)
        UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
        UIMSG_WM_LBUTTONUP(OnLButtonUp)
        UIMSG_WM_RBUTTONUP(OnRButtonUp)
        UIMSG_WM_MOUSEWHEEL(OnMouseWheel)
        UIMSG_WM_KEYDOWN(OnKeyDown)
        UIMSG_WM_KEYUP(OnKeyUp)

        UIMSG_WM_DROPTARGETEVENT(OnDropTargetEvent)
        UIMESSAGE_HANDLER_EX(WM_USER_REFRESH, OnRefresh)

        UIMSG_WM_KILLFOCUS(OnKillFocus)
        UIMSG_WM_CANCELMODE(OnCancelMode)

        UIMESSAGE_HANDLER_EX(MDIVIEW_MESSAGE_PREVIEW, OnBtnClickPreView)
		UIMESSAGE_HANDLER_EX(UI_WM_PREVIEW_WINDOW_DESTROY, OnPrevViewWindowDestroy)
        UIMSG_WM_PRECREATEWINDOW(PreCreateWindow)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

public:
    BOOL PreCreateWindow(CREATESTRUCT* pcs);
    void OnPaint(IRenderTarget* pRenderTarget);
    void OnEraseBkgnd(IRenderTarget* pRenderTarget);
	void OnInitialize();
	void DoPaintBkgnd(CDCHandle& dc);
	void DoPaint(CDCHandle& dc);
    void OnSize(UINT nType, int cx, int cy);
    LRESULT  OnRefresh(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    void OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags );
    BOOL OnSetCursor(HWND wnd, UINT nHitTest, UINT message);

    void OnMouseMove(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
    void OnLButtonUp(UINT nFlags, CPoint point);
	void OnRButtonUp(UINT nFlags, CPoint point);
    BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    void OnKillFocus(IObject* pNewFocusObj);
    void OnCancelMode();

    void OnDropTargetEvent(DROPTARGETEVENT_TYPE eType, DROPTARGETEVENT_DATA* pData);
	LRESULT OnBtnClickPreView(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnPrevViewWindowDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam);

#pragma  region
    virtual void**  QueryInterface(REFIID riid);
	virtual ISkinRes*  GetSkinRes(){ return m_pSkin; }
	virtual void   SetSkinRes(ISkinRes* hSkin) { m_pSkin = hSkin; }

	virtual UI_RESOURCE_TYPE  GetResourceType(){ return UI_RESOURCE_LAYOUT; }
    virtual HWND  GetHWND() { return IWindow::GetHWND(); }
	virtual void  OnMDIActivate(bool bActivate);
	virtual long*  GetKey();

	virtual bool ShowLayoutNode(LayoutTreeItemData* pData);
    virtual IWindow*  GetWindow() { return m_pWindow; }
	virtual void  OnObjectDeleteInd(IObject* p);
#pragma  endregion

#pragma region
    virtual void  OnRectTrackerDragStart();
    virtual void  OnRectTrackerDrag(RectTrackerHit eHit, int xDelta, int yDelta);
    virtual void  OnRectTrackerDragEnd();
    virtual void  DP2LP(POINT* p);
#pragma endregion

protected:
	IObject*  CreateObject(const TCHAR* szNodeName, const TCHAR* szNodeID);
	void     LoadObjectChildNode(IObject* pParent);

	void  SelectObject(IObject* pObject);
    void  PrepareDC(HDC hDC);
    void  UnPrepareDC(HDC hDC);

    IObject*  GetContainerObject(IObject* pCurObj);
	void  UpdateTitle();

    void  SetCurSelectObject(IObject*  pSelectObj);
    void  SetWindowDrawRect(RECT* prc);
    void  UpdateRectTracker();
    
private:
    LayoutPropertyHandler  m_PropertyHandler;
	HBRUSH  m_hBkBrush;

	ISkinRes*  m_pSkin;
	IWindow*   m_pWindow;
	IObject*   m_pCurSelectObject;  // 给该变量赋值必须使用SetCurSelectObject

    HCURSOR  m_hCursorHand;
    HCURSOR  m_hCursorHandPress;
    
	RECT  m_rcWindowDraw;  // UI窗口绘制的最终区域, 逻辑坐标。给该变量赋值必须调用SetWindowDrawRect
    int   m_nZoom;     // 放大参数  100%
    bool  m_bCenter;   // 居中绘制

    RectTracker  m_recttracker;
    CRect  m_rectObjectSave;    // 拖拽控件大小前保存该控件位置

    POINT  m_ptLastClick;
    POINT  m_ptLastMouseMove;
    MOUSE_ACTION  m_eMouseAction;
};

#if 0
//
//	样式layout的代码编辑窗口
//
class CLayoutCodeDlg : public CDialogImpl<CLayoutCodeDlg>, public CResourceCodeDlg<CLayoutCodeDlg>
{
public:
	enum { IDD = IDD_DIALOG_LAYOUT_CODE };

	CLayoutCodeDlg();
	~CLayoutCodeDlg();

	BEGIN_MSG_MAP_EX(CLayoutCodeDlg)
		CHAIN_MSG_MAP(CResourceCodeDlg)
	END_MSG_MAP()

public:
};


class CLayoutBuildDlg : public CDialogImpl<CLayoutBuildDlg>
{
public:
	enum {IDD=IDD_DIALOG_LAYOUT_BUILD};
	CLayoutBuildDlg();
	~CLayoutBuildDlg();

	virtual void OnFinalMessage(HWND /*hWnd*/)
	{
		// 由于在childframe中不能很好的处理各种builddlg的释放，因此将释放操作放在这里进行
		delete this;
	}

	BEGIN_MSG_MAP_EX( CLayoutBuildDlg )
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_SIZE(OnSize)
		NOTIFY_HANDLER_EX(IDC_TAB, TCN_SELCHANGING, OnSelChanging )
	END_MSG_MAP()

public:
	void      AttachSkin( HSKIN hSkin );
	HSKIN     GetSkin() { return m_pSkin; }

protected:
	BOOL      OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void      OnSize(UINT nType, CSize size);
	LRESULT   OnSelChanging(LPNMHDR pnmh);

private:
	CTabCtrl        m_tabCtrl;
	CLayoutCodeDlg*  m_pLayoutCodePanel;
	CLayoutViewDlg*  m_pLayoutViewPanel;
	HSKIN           m_pSkin;
};
#endif