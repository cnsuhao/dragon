#pragma once
#include "UISDK\Kernel\Inc\Interface\iwindow.h"

#include "UISDK\Project\UI3D\inc\inc.h"
#pragma comment(lib, "UI3D.lib")
#include "UISDK\Project\UI3D\inc\istage3d.h"

namespace UI
{
}
class CDirect2DWindow : public UI::IWindow
{
public:
    CDirect2DWindow();
	~CDirect2DWindow(void);

 	UI_BEGIN_MSG_MAP_Ixxx(CDirect2DWindow)
        UIMSG_WM_INITIALIZE(OnInitWindow)
 	UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

 	void  OnInitWindow();
};

