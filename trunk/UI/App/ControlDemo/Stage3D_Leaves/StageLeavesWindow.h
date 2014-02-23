#pragma once
#include "UISDK\Kernel\Inc\Interface\iwindow.h"

#include "UISDK\Project\UI3D\inc\inc.h"
#pragma comment(lib, "UI3D.lib")
#include "UISDK\Project\UI3D\inc\istage3d.h"

#include "uisdk/Control/Inc/Interface/iedit.h"

//
//
// http://www.webkit.org/blog-files/leaves/index.html
// ��CSS3����Ҷ
//
// TODO: ���յ�ʵ��Ч������û��ԭ�������
// m_pDevice->GetRenderTargetData�����ʱ�ĵط�
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

	float  m_fyWorldDropFrom; // ��Ҷ��ʼ����ĸ߶�
	float  m_fyWorldDropTo;  // ��Ҷ���䵽�ļ۸�
	float  m_fyWolrdDropToFade;  // ��Ҷ��ʼ����������
	float  m_fyWolrdDropFadeHeight;  // ��Ҷ�����ĸ߶�
};

