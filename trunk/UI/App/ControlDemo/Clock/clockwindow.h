#pragma once
#include "UISDK\Kernel\Inc\Interface\iwindow.h"


//
// ʱ��ģ��
// 1. �޸�ROTATE_3D�궨�壬������ʹ��2D��ת��������3D��ת
// 2. �޸�clock.xml�е�graphics="gdi/gdiplus/d2d" ��Ⱦ������
// 3.  gdi������ݣ�gdiplus��d3d��soft3d֧�ֿ����
//

// ������ʹ�����3D��ת������ʹ��2D������ת
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

