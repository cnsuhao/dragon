#pragma once
#include "UISDK\Kernel\Inc\Interface\iwindow.h"

namespace UI
{
    interface IPictureCtrl;
    interface IButton;
}
class CControlDemoWindow : public UI::IWindow
{
public:
    CControlDemoWindow();
	~CControlDemoWindow(void);

 	UI_BEGIN_MSG_MAP_Ixxx(CControlDemoWindow)
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_INITIALIZE(OnInitWindow)
        UIMESSAGE_HANDLER_EX(UI_WM_DROPTARGETEVENT, OnDropTargetEvent)
        UIMSG_BN_CLICKED3(OnBnClicked)
        UIMSG_WM_GETGRAPHICSRENDERLIBRARYTYPE(OnGetGraphicsRenderType)
        UIMSG_WM_TIMER(OnTimer)
 	UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

 	void  OnInitWindow();
    LRESULT OnDropTargetEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void  OnPaint(UI::IRenderTarget* pRenderTarget);
    void  OnTimer(UINT_PTR nIDEvent, LPARAM lParam);
    UI::GRAPHICS_RENDER_LIBRARY_TYPE OnGetGraphicsRenderType()
    {
        return UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS;
    }

    void  OnBnClicked(IMessage*  pMsgFrom);
	void  Start();
	void  End();

    UI::IPictureCtrl*  m_pCtrl;
	UI::IPictureCtrl*  m_pCtrl2;
	UI::IPictureCtrl*  m_pCtrl3;
    UI::IButton* m_pBtnStart;
    UI::IButton* m_pBtnEnd;
};

