#pragma once
#include "UISDK\Kernel\Inc\Interface\iwindow.h"

namespace UI
{
    interface IPictureCtrl;
}
class REShadowWindow : public UI::IWindow
{
public:
    REShadowWindow();
	~REShadowWindow(void);

 	UI_BEGIN_MSG_MAP_Ixxx(REShadowWindow)
        UIMSG_WM_INITIALIZE(OnInitWindow)
        UIMSG_WM_TIMER(OnTimer)
        UIMSG_WM_GETGRAPHICSRENDERLIBRARYTYPE(OnGetGraphicsRenderType)
 	UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

 	void  OnInitWindow();
    void  OnTimer(UINT_PTR nIDEvent, LPARAM lParam);
    UI::GRAPHICS_RENDER_LIBRARY_TYPE OnGetGraphicsRenderType()
    {
        return UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS;
    }

    void  CheckTime();

    UI::IPictureCtrl*  m_pHour;
    UI::IPictureCtrl*  m_pMin;
    UI::IPictureCtrl*  m_pSec;

    int m_nHourRotate;
    int m_nMinRotate;
    int m_nSecRotate;
};

