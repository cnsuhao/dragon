#pragma once
#include "Other\resource.h"
#include "Dialogs.h"
#include "Dialog\Framework\childframe.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Project\UIEditor\Util\trackrect.h"

class CMenuViewDlg : public IWindow,
                     public IMenuMDIClientView
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

	CMenuViewDlg();
	~CMenuViewDlg();

    UI_BEGIN_MSG_MAP_Ixxx(CMenuViewDlg)
        if (m_recttracker.ProcessMessage(uMsg, wParam, lParam, &lResult))
        {
            return TRUE;
        }

        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
        UIMSG_WM_SIZE(OnSize)
        UIMESSAGE_HANDLER_EX(WM_USER_REFRESH, OnRefresh)

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

	LRESULT OnBtnClickPreView(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnPrevViewWindowDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void  DP2LP(POINT* p);

#pragma  region
    virtual void**  QueryInterface(REFIID riid);
	virtual ISkinRes*  GetSkinRes(){ return m_pSkin; }
	virtual void   SetSkinRes(ISkinRes* hSkin) { m_pSkin = hSkin; }

	virtual UI_RESOURCE_TYPE  GetResourceType(){ return UI_RESOURCE_MENU; }
    virtual HWND  GetHWND() { return IWindow::GetHWND(); }
	virtual void  OnMDIActivate(bool bActivate);
	virtual long*  GetKey();

	virtual bool ShowLayoutNode(LayoutTreeItemData* pData);
    virtual IMenu*  GetMenu() { return m_pMenu; }
#pragma  endregion


protected:
	IObject*  CreateObject(const TCHAR* szNodeName, const TCHAR* szNodeID);
	void     LoadObjectChildNode(IObject* pParent);

    void  PrepareDC(HDC hDC);
    void  UnPrepareDC(HDC hDC);

	void  UpdateTitle();

    void  SetWindowDrawRect(RECT* prc);
    
private:
	HBRUSH  m_hBkBrush;

	ISkinRes*  m_pSkin;
	IMenu*     m_pMenu;    // �����һ���˵������ֵ��Ч��ͬʱm_pWindowΪ�˵��İ�װ����

	RECT  m_rcWindowDraw;  // UI���ڻ��Ƶ���������, �߼����ꡣ���ñ�����ֵ�������SetWindowDrawRect
    RectTracker  m_recttracker;
};
