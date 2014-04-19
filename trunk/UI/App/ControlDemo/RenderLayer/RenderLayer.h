#pragma once
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"

namespace UI
{
    interface IButton;
    interface IFlashWrap;
	interface ISliderCtrl;
}

class Cxxx : public UI::ICustomWindow
{
public:
	UI_BEGIN_MSG_MAP_Ixxx(Cxxx)
 	UI_END_MSG_MAP_CHAIN_PARENT(ICustomWindow)
};
class CRenderLayerWindow : public UI::IWindow
{
public:
    CRenderLayerWindow();
	~CRenderLayerWindow(void);

 	UI_BEGIN_MSG_MAP_Ixxx(CRenderLayerWindow)
        UIMSG_WM_INITIALIZE(OnInitWindow)
        UIMSG_BN_CLICKED2(m_pButtonSwitchFlash, OnBnClick)
        UIMSG_BN_CLICKED2(m_pButtonJump, OnBnClickJump)
        UIMSG_TRBN_POSCHANGED_ID(_T("slider"), OnTrbnPosChanged)
 	UI_END_MSG_MAP_CHAIN_PARENT(IWindow)

 	void  OnInitWindow();
    void  OnBnClick();
    void  OnBnClickJump();
    void  OnTrbnPosChanged(int nPos, int nScrollType);

    void  SwitchFlash();
private:
    UI::IButton*  m_pButtonSwitchFlash;
    UI::IButton*  m_pButtonJump;
    UI::IFlashWrap*  m_pFlash;
	UI::IPanel*  m_pPanel;
	UI::ISliderCtrl*  m_pSlider;

    vector<String>  m_vecFlashUri;
};

