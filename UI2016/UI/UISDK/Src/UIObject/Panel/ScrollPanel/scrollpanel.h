#pragma once
#include "UISDK\Kernel\Inc\Interface\iscrollpanel.h"
#include "UISDK\Kernel\Src\UIObject\ScrollBar\scrollbarmanager.h"
#include "UISDK\Kernel\Src\UIObject\Panel\panel.h"

namespace UI
{

class ScrollPanel : public Panel
{
public:
	ScrollPanel(IScrollPanel* p);
	~ScrollPanel();

	UI_BEGIN_MSG_MAP
        UIMSG_WM_MOUSEWHEEL(OnMouseWheel)
        UICHAIN_MSG_MAP_MEMBER(m_mgrScrollBar)
        UIMSG_WM_SERIALIZE(OnSerialize)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
	UI_END_MSG_MAP_CHAIN_PARENT(Panel)

    IScrollPanel*  GetIScrollPanel() { return m_pIScrollPanel; }

protected:
	virtual void  virtualOnSize(UINT nType, UINT nWidth, UINT nHeight);

    HRESULT  FinalConstruct(IUIApplication* p);
	void  OnSerialize(SERIALIZEDATA* pData);
    BOOL  OnMouseWheel(UINT nFlags, short zDelta, POINT pt);

protected:
    IScrollPanel*      m_pIScrollPanel;
	ScrollBarManager   m_mgrScrollBar;

    SIZE    m_sizeView;  // 视图大小（包含滚动区域），如果未指定，则获取所有子控件的位置进行计算
};
}