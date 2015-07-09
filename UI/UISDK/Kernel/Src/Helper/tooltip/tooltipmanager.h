#ifndef TOOLTIPMANAGER_H_8861C608_0281_42fc_B665_0F005E07D027
#define TOOLTIPMANAGER_H_8861C608_0281_42fc_B665_0F005E07D027

#include "UISDK\Kernel\Src\Util\Timeout\timeout.h"

namespace UI
{
#define TOOL_TIMER_TIME    600
#define TOOLTIP_MAX_WIDTH  500/*256*/


//////////////////////////////////////////////////////////////////////////
// TimerProc thunk

class ToolTipItem
{
public:
    ToolTipItem();

public:
    void  Set(TOOLTIPITEM* pItem);
    void  Reset();
    
public:
    String  strContent;

    IMessage*  pNotifyObj;   
    void*  pItemData;    

    int  nToolTipFlag; 
};

//
// 提示条的管理
// 用于管理各种计时器及消息转发，并实现各种逻辑，使得IToolTipUI和控件不用去管这些逻辑
//
class ToolTipManager : public ITimeoutCallback
{
public:
	ToolTipManager();
	~ToolTipManager();

	void   Init(UIApplication* p, IToolTipUI* pTooltipUI=NULL);
	void   Release();
	bool   Hide();
	bool   Show(TOOLTIPITEM* pItemInfo);

protected:
    virtual void OnTimeout(long lId, WPARAM wParam, LPARAM lParam) override;

protected:
	IToolTipUI*        m_pToolTipUI;      // 就是控件的tooltip
	ToolTipItem        m_tooltipItem;     // 当前正在显示或者正在等待显示的提示条信息
    UIApplication*     m_pUIApplication;

    CTimeout   m_timeout;
};

}

#endif // TOOLTIPMANAGER_H_8861C608_0281_42fc_B665_0F005E07D027