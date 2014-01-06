#pragma once
#include "..\wndtransmodebase.h"
namespace UI
{

//
//	分层窗口实现代码
//
class LayeredWindowWrap : public WndTransModeBase
{
public:
    LayeredWindowWrap();
    ~LayeredWindowWrap();

    VIRTUAL_BEGIN_MSG_MAP(LayeredWindowWrap)
        MESSAGE_HANDLER(WM_WINDOWPOSCHANGED, _OnWindowPosChanged)
        MESSAGE_HANDLER(WM_CANCELMODE, _OnCancelMode)
    END_MSG_MAP()


    virtual BOOL  ProcessMessage(UIMSG* pMsg, int nMsgMapID, bool bDoHook)
    {
        return innerVirtualProcessMessage(pMsg, nMsgMapID, bDoHook);
    }
    UI_BEGIN_MSG_MAP
        UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
        UIMSG_WM_LBUTTONUP(OnLButtonUp)
        UIMSG_WM_MOUSEMOVE(OnMouseMove)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
    UI_END_MSG_MAP

    LRESULT  _OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  _OnCancelMode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    void  OnLButtonDown(UINT nFlags, POINT point);
    void  OnLButtonUp(UINT nFlags, POINT point);
    void  OnMouseMove(UINT nFlags, POINT point);
    void*  QueryInterface(const IID* pIID);

    ILayeredWindowWrap*  GetILayeredWindowWrap();
    byte  GetAlpha();
    void  SetAlpha(byte b, bool bUpdate);

    virtual WINDOW_TRANSPARENT_MODE  GetModeValue()
    {
        return WINDOW_TRANSPARENT_MODE_LAYERED;
    }
    virtual bool  RequireAlphaChannel()
    {
        return true;
    }
    virtual void  Init(ICustomWindow* pWnd)
    {
        __super::Init(pWnd);
        InitLayeredWindow();
    }
    virtual void  UpdateRgn() 
    {}
    virtual bool  Commit();
    virtual void  Enable(bool b);

    void      InitLayeredWindow();
    void      OnWindowPosChanged(LPWINDOWPOS lpWndPos);

    void      OnLButtonDown(UINT nHitTest);
    void      OnEnterSizeMove(UINT nHitTest);
    void      OnExitSizeMove();

protected:
    bool      IsMinimized();

protected:
    friend    class CustomWindow;
    ILayeredWindowWrap*  m_pILayeredWindowWrap;

    POINT     m_ptWindow;          // 分层窗口的坐标
    SIZE      m_sizeWindow;        // 分层窗口的大小
    byte      m_nAlphaValue;       // 窗口整体透明度

    // 分层窗口拉伸时使用的中间参数
    UINT      m_nHitTestFlag;      // 拉伸标识
    POINT     m_ptStartSizeMove;   // 开始拉伸时，鼠标的位置，用于计算鼠标偏移
    POINT     m_ptWindowOld;       // 开始拉伸时的窗口坐标，用于和偏移一起计算新位置
    SIZE      m_sizeWindowOld;     // 开始拉伸时的窗口大小，用于和偏移一起计算新大小
};

}