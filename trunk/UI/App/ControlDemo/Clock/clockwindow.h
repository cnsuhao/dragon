#pragma once
#include "UISDK\Kernel\Inc\Interface\iwindow.h"


//
// 时钟模拟
// 1. 修改ROTATE_3D宏定义，决定是使用2D旋转，还是软3D旋转
// 2. 修改clock.xml中的graphics="gdi/gdiplus/d2d" 渲染库类型
// 3.  gdi不抗锯齿，gdiplus、d3d、soft3d支持抗锯齿
//

// 定义是使用软件3D旋转，还是使用2D仿射旋转
// #define ROTATE_3D

namespace UI
{
    interface IPictureCtrl;
	interface IRenderLayer2;
}
class ClockWindow : public UI::IWindow
{
public:
    ClockWindow();
	~ClockWindow(void);

 	UI_BEGIN_MSG_MAP_Ixxx(ClockWindow)
        UIMSG_WM_INITIALIZE(OnInitWindow)
        UIMSG_WM_TIMER(OnTimer)
		UIMSG_WM_PAINT(OnPaint)
 	UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

 	void  OnInitWindow();
    void  OnTimer(UINT_PTR nIDEvent, LPARAM lParam);
    void  CheckTime();
	void  OnPaint(UI::IRenderTarget* pRenderTarget);

    UI::IPictureCtrl*  m_pHour;
    UI::IPictureCtrl*  m_pMin;
    UI::IPictureCtrl*  m_pSec;

	UI::IRenderLayer2*  m_pHourLayer;
	UI::IRenderLayer2*  m_pMinLayer;
	UI::IRenderLayer2*  m_pSecLayer;

    int m_nHourRotate;
    int m_nMinRotate;
    int m_nSecRotate;
};

