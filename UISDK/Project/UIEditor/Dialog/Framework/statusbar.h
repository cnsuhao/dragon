#pragma once
#include "UISDK\Kernel\Inc\Interface\iwindow.h"

namespace UI
{
    interface ILabel;
}
class CStatusBar : public IWindow
{
public:
    CStatusBar();
    ~CStatusBar();

    UI_BEGIN_MSG_MAP_Ixxx(CStatusBar)
        UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
        UIMSG_WM_TIMER(OnTimer)
        UIMSG_WM_INITIALIZE(OnInitialize)
        UIMSG_WM_PRECREATEWINDOW(PreCreateWindow)
    UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

    void  SetStatusText(const TCHAR*  szText);
    void  SetStatusText2(const TCHAR*  szText, int nDuration=10000);

protected:
    BOOL  PreCreateWindow(CREATESTRUCT* pcs);
    void  OnInitialize();
    void  OnTimer(UINT_PTR nIDEvent, LPARAM lParam);
    void  OnEraseBkgnd(IRenderTarget* pRenderTarget);

private:
    ILabel*   m_pLabel;
    UINT_PTR  m_nTimerId;
};

