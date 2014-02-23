#pragma once
#include "UISDK\Kernel\Inc\Interface\iwindow.h"

#include "UISDK\Project\UI3D\inc\inc.h"
#pragma comment(lib, "UI3D.lib")
#include "UISDK\Project\UI3D\inc\istage3d.h"

namespace UI
{
    interface IButton;
    interface IStoryboard;
}
class CAnimateWindow : public UI::IWindow
{
public:
    CAnimateWindow();
	~CAnimateWindow(void);

 	UI_BEGIN_MSG_MAP_Ixxx(CAnimateWindow)
        UIMSG_WM_INITIALIZE(OnInitWindow)
        UIMSG_BN_CLICKED2(m_pButton, OnBnClick)
        UIMSG_WM_ANIMATE_TICK(OnAnimateTick)
        UIMSG_WM_ANIMATE_OVER(OnAnimateOver)
 	UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

 	void  OnInitWindow();
    void  OnBnClick();
    void  OnAnimateTick(int nCount, UI::IStoryboard** ppArray);
    void  OnAnimateOver();

private:
    UI::IButton*  m_pButton;
};

