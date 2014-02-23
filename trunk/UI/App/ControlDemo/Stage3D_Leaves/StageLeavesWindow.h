#pragma once
#include "UISDK\Kernel\Inc\Interface\iwindow.h"

#include "UISDK\Project\UI3D\inc\inc.h"
#pragma comment(lib, "UI3D.lib")
#include "UISDK\Project\UI3D\inc\istage3d.h"

#include "uisdk/Control/Inc/Interface/iedit.h"

//
//
// http://www.webkit.org/blog-files/leaves/index.html
// 仿CSS3，落叶
//
// TODO: 最终的实现效果还是没有原版的流畅
// m_pDevice->GetRenderTargetData是最耗时的地方
//
//
namespace UI
{
    interface IStoryboard;
}
class CStageLeavesWindow : public UI::IWindow
{
public:
    CStageLeavesWindow();
	~CStageLeavesWindow(void);

 	UI_BEGIN_MSG_MAP_Ixxx(CStageLeavesWindow)
        UIMSG_WM_ANIMATE_TICK(OnAnimateTick)
        UIMSG_WM_INITIALIZE(OnInitWindow)
 	UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

 	void  OnInitWindow();
    void  OnAnimateTick(int nCount, UI::IStoryboard** ppArray);

protected:
    void  ResetLeaf(UI::IImageElement* pElem);

private:
    UI::IStage3D*  m_pStage3D;

	float  m_fyWorldDropFrom; // 树叶开始降落的高度
	float  m_fyWorldDropTo;  // 树叶降落到的价格
	float  m_fyWolrdDropToFade;  // 树叶开始渐隐的坐标
	float  m_fyWolrdDropFadeHeight;  // 树叶渐隐的高度
};

