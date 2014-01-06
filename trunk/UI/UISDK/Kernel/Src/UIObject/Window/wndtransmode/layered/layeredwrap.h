#pragma once
#include "..\wndtransmodebase.h"
namespace UI
{

//
//	�ֲ㴰��ʵ�ִ���
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

    POINT     m_ptWindow;          // �ֲ㴰�ڵ�����
    SIZE      m_sizeWindow;        // �ֲ㴰�ڵĴ�С
    byte      m_nAlphaValue;       // ��������͸����

    // �ֲ㴰������ʱʹ�õ��м����
    UINT      m_nHitTestFlag;      // �����ʶ
    POINT     m_ptStartSizeMove;   // ��ʼ����ʱ������λ�ã����ڼ������ƫ��
    POINT     m_ptWindowOld;       // ��ʼ����ʱ�Ĵ������꣬���ں�ƫ��һ�������λ��
    SIZE      m_sizeWindowOld;     // ��ʼ����ʱ�Ĵ��ڴ�С�����ں�ƫ��һ������´�С
};

}