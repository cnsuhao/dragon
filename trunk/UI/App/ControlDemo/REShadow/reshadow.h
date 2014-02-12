#pragma once
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
namespace UI
{
    interface IRichEdit;
}

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
 	UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

 	void  OnInitWindow();
    UI::IRichEdit*  m_pRichEdit;
};

