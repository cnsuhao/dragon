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
// 2014.4.9 放弃使用3D绘制，修改为直接使用2D旋转
//

// 树叶数量
#define LEAVES_COUNT  30

namespace UI
{
    interface IStoryboard;
    interface IPictureCtrl;
    interface IPanel;
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
    void  ResetLeaf(UI::IPictureCtrl* pImg);

private:
    UI::IStage3D*  m_pStage3D;
    UI::IPictureCtrl*  m_pImgs[LEAVES_COUNT];
    UI::IPanel*  m_pLeavesPanel;

	float  m_fyWorldDropFrom;        // 树叶开始降落的高度
	float  m_fyWorldDropTo;          // 树叶降落到的价格
	float  m_fyWolrdDropToFade;      // 树叶开始渐隐的坐标
	float  m_fyWolrdDropFadeHeight;  // 树叶渐隐的高度
};

