#pragma once
#include "UISDK\Kernel\Inc\Interface\iwindow.h"

#include "UISDK\Project\UI3D\inc\inc.h"
#pragma comment(lib, "UI3D.lib")
#include "UISDK\Project\UI3D\inc\istage3d.h"

namespace UI
{
}
class CStage3DWindow : public UI::IWindow
{
public:
    CStage3DWindow();
	~CStage3DWindow(void);

 	UI_BEGIN_MSG_MAP_Ixxx(CStage3DWindow)
        UIMSG_WM_INITIALIZE(OnInitWindow)
 	UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

 	void  OnInitWindow();

private:
    UI::IStage3D*  m_pStage3D;
};

