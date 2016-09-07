#ifndef IWINDWOBASE_H_45FADAE6_FB0B_49e0_8119_6F6D58FD5700
#define IWINDWOBASE_H_45FADAE6_FB0B_49e0_8119_6F6D58FD5700

#include "irenderlayer.h"
#include "ipanel.h"

namespace UI
{
interface IWindowMouseMgr;
interface IWindowKeyboardMgr;

#define ANCHOR_NONE        0
#define ANCHOR_LEFT        0x0001
#define ANCHOR_RIGHT       0x0002
#define ANCHOR_TOP         0x0004
#define ANCHOR_BOTTOM      0x0008
#define ANCHOR_OUT_LEFT    0x0010
#define ANCHOR_OUT_RIGHT   0x0020
#define ANCHOR_OUT_TOP     0x0040
#define ANCHOR_OUT_BOTTOM  0x0080
#define ANCHOR_CUSTOM      0x0100   // 发送UI_WM_SYNC_WINDOWPOSCHANGING消息给窗口，由窗口自行决定如何移动自己


enum SYNC_WINDOW_EVENT_TYPE
{
    ADD_SYNC_WINDOW,            // 通知HOST, 增加一个跟随者.LPARAM: SyncWindowData*
    MODIFY_SYNC_WINDOW,         // 通知HOST, 修改一个跟随者信息.LPARAM: SyncWindowData*
    REMOVE_SYNC_WINDOW,         // 通知HOST, 移除一个跟随者.LPARAM: SyncWindowData*

    ADD_SYNC_WINDOW_RESULT,     // 通知SITE，添加成功.LPARAM: host HWND
    MODIFY_SYNC_WINDOW_RESULT,  // 通知SITE，修改成功.LPARAM: host HWND
    REMOVE_SYNC_WINDOW_RESULT,  // 通知SITE，删除成功.LPARAM: host HWND

    HOST_WINDOW_DESTROYED,      // 要跟随的HOST销毁了，清空自己的HOST句柄. LPARAM: 无
    HOST_WINDOW_POSCHANGING,    // 发送给SITE窗口，标明下一次响应的WindowPosChaning消息是因为HOST移动产生的
};

struct  AnchorData
{
    int  xOffset;
    int  yOffset;

    union{
        int xOffset2;
        int Width;         // -1 表示不修改窗口宽度
    };
    union{
        int yOffset2;
        int Height;       // -1 表示不修改窗口高度
    };

    AnchorData()
    {
        xOffset = yOffset = xOffset2 = yOffset2 = 0;
    }
};

#define SWDS_MASK_ANCHORTYPE 0x0001
#define SWDS_MASK_ANCHORDATA 0x0002
#define SWDS_MASK_ANCHORON   0x0004
#define SWDS_MASK_ALL   (SWDS_MASK_ANCHORTYPE|SWDS_MASK_ANCHORDATA|SWDS_MASK_ANCHORON)
struct  SyncWindowData
{
    HWND         m_hWnd;           
    UINT         m_nMask;
    UINT         m_nAnchorType;   
    AnchorData   m_rcAnchorData;  // 当m_nAnchorType相应位有值时，m_rcAnchorData的该位有效
    bool         m_bAnchorOn;     // 跟随是否起作用。例如临时将窗口移出HOST窗口，不进行跟随，然后又贴到HOST窗口旁再次开启跟随。
	bool         m_bSyncVisible;  // 是否同步显隐

    SyncWindowData()
    {
        m_hWnd = NULL;
        m_nAnchorType = ANCHOR_NONE;
        m_bAnchorOn = true;
		m_bSyncVisible = false;

        m_nMask = 0;
    }
};

// window style
typedef struct tagWindowStyle
{
	bool  destroyed : 1;       // 表示该窗口已经被销毁了(WM_NCDESTROY)，用于触发OnFinalMessage
	bool  attach : 1;          // 表示该窗口是attach的，创建、销毁由外部来控制
	bool  setcreaterect : 1;   // 创建窗口时指定了窗口大小，不用再进行窗口布局了
	bool  dialog_noresize : 1; // 用于解决win7下面Dialog显示大小于GetWindowRect不一致的问题
	bool  hard_composite: 1;   // 本窗口使用硬件合成 
}WindowStyle;

// 用于外部业务实现者处理窗口消息。
// 外部不再要求从IWindowBase派生，只需要实现IWindowMessageCallback接口即可。
interface IWindowMessageCallback
{
	virtual BOOL  OnWindowMessage(UINT, WPARAM, LPARAM, LRESULT& lResult) = 0;
	virtual BOOL  OnWindowUIMessage(UIMSG* pMsg) = 0;
};

class WindowBase;
interface __declspec(uuid("1C7CED21-3CF6-49C9-9E52-72522C8A1CF6"))
UISDKAPI IWindowBase : public IPanel
{
    HWND  GetHWND();
    IWindowRender*  GetIWindowRender();
	ISkinRes*  GetSkinRes();

    BOOL  IsChildWindow();
	BOOL  IsWindowVisible();
    void  ShowWindow();
    void  HideWindow();
    bool  IsDoModal();
    void  CenterWindow(HWND hWndCenter = NULL);
    void  CenterWindow(HMONITOR hMonitor = NULL);

    void  SetFocusObject(IObject* pObj);
	void  SetPressObject(IObject* pObj);
    IObject*  GetHoverObject();
    IObject*  GetPressObject();
    IObject*  GetObjectByPos(IObject* pObjParent, POINT* pt, bool bSkinBuilderInvoke=false);

    bool  Create(LPCTSTR szID, HWND hWndParent=NULL, RECT* prc=NULL, long lStyle = 0, long lExStyle = 0);
    bool  Attach(HWND hWnd, LPCTSTR szID);
    void  Detach();
    long  DoModal(LPCTSTR szID, HWND hWndParent, bool canResize);
    long  DoModal(HINSTANCE hResInst, UINT nResID, LPCTSTR szID, HWND hWndParent);
    HWND  DoModeless(LPCTSTR szID, HWND hWndParent, bool canResize);
    HWND  DoModeless(HINSTANCE hResInst, UINT nResID, LPCTSTR szID, HWND hWndParent);
    void  EndDialog(INT_PTR);
	void  DestroyWindow();

    void  UpdateWindow(HDC hDC, RECT* prcInvalid);
    void  SaveMemBitmap(TCHAR* szFile);
    void  DrawMemBitmap(HDC hDC, RECT* prc, bool bAlphaBlend);
    void  EnableDwmTransition(bool b);
	void  EnableGpuComposite(bool b);
	bool  IsGpuComposite();
	void  DirectComposite();
	void  SetWindowMessageCallback(IWindowMessageCallback*);

    void  CalcWindowSizeByClientSize( SIZE sizeClient, SIZE* pSizeWindow );
    void  CalcClientRectByWindowRect( RECT* rcWindow, RECT* rcClient );

    bool  AddAnchorItem(const SyncWindowData& data);
    void  HideAllAnchorItem();

    HRESULT  SetDroppable(bool b);

    UI_DECLARE_INTERFACE(WindowBase);
};

class Window;
interface __declspec(uuid("5C36801E-5929-4512-A998-F9719DCC6903"))
UISDKAPI IWindow : public IWindowBase
{
    UI_DECLARE_INTERFACE(Window)
};

}

#endif  // IWINDWOBASE_H_45FADAE6_FB0B_49e0_8119_6F6D58FD5700