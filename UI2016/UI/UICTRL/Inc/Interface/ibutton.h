#ifndef _UI_CTRL_IBUTTON_H_
#define _UI_CTRL_IBUTTON_H_

#include "..\..\..\UISDK\Inc\Interface\icontrol.h"

namespace UI
{
	struct BUTTONSTYLE
	{
		bool  default_push_button : 1;   // 可以成为默认按钮
		bool  click_on_pushdown : 1;     // 当鼠标按下时就触发click，而不是鼠标弹起时触发
		bool  notify_hoverleave : 1;     // 鼠标移入移出时，发出通知
		bool  hand_cursor : 1;           // 使用手型鼠标
	};

	enum BUTTON_TYPE
	{
		// button style
		BUTTON_TYPE_HAVE_TEXT_FIRST = 0x00,      // 标志有内容的按钮。这些按钮才需要创建字体textrender
		BUTTON_TYPE_PUSHBUTTON = 0x00,           // 普通按钮
		BUTTON_TYPE_RADIOBUTTON = 0x01,          // 单选按钮
		BUTTON_TYPE_CHECKBUTTON = 0x02,          // 复选按钮
		BUTTON_TYPE_HYPERLINK = 0x03,            // 超链接按钮
		BUTTON_TYPE_MENUBUTTON = 0x04,           // 菜单按钮
		BUTTON_TYPE_SPLITMENUBUTTON = 0x05,      // 按钮+菜单
		BUTTON_TYPE_TABCTRLBUTTON = 0x06,        // TAB控件上的迎宾馆
		BUTTON_TYPE_HAVE_TEXT_LAST = 0x80,

		BUTTON_TYPE_COMBOBOX = 0x81,             // 组合框中的按钮
		BUTTON_TYPE_SCROLLLINEUP = 0x82,         // 滚动条按钮
		BUTTON_TYPE_SCROLLLINEDOWN = 0x83,
		BUTTON_TYPE_SCROLLLINELEFT = 0x84,
		BUTTON_TYPE_SCROLLLINERIGHT = 0x85,
		BUTTON_TYPE_HSCROLLTHUMB = 0x86,         // 滚动条拖拽按钮
		BUTTON_TYPE_VSCROLLTHUMB = 0x87,         // 滚动条拖拽按钮
		BUTOTN_TYPE_SLIDERTRACKBUTTON = 0x88,    // 滑动条上面的按钮
	};

class ButtonBase;
interface UICTRLAPI IButtonBase : public IControl
{
    void  SetCheck(int nCheckState);
    void  SetCheck(bool bCheck, bool bUpdate);
    void  SetChecked();
    void  SetUnChecked();
    int   GetCheck();
    bool  IsChecked();
	void  SetButtonType(BUTTON_TYPE n);

	UI_DECLARE_INTERFACE_ACROSSMODULE(ButtonBase)
};

class Button;
interface __declspec(uuid("DEBF57B2-C7C4-480B-A90D-427891E77502"))
UICTRLAPI IButton : public IButtonBase
{
    void  SetText(LPCTSTR  szText, bool bUpdate=true);
    LPCTSTR GetText();
    
    void  SetIconFromFile(LPCTSTR szIconPath);
    void  SetIconFromHBITMAP(HBITMAP hBitmap);   // 注：该HBITMAP最后由内部销毁

    void  SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE eType);
    void  SetAutoSizeType(BUTTON_AUTO_SIZE_TYPE eType);
	
    signal<IButton*>&  ClickedEvent();

	UI_DECLARE_INTERFACE(Button);
};

//
//	按钮点击事件
//
//		message:UI_WM_NOTIFY
//		code:   UI_BN_CLICKED
//		wparam: NA // -- POINT*，基于window
//		lParam: 
//
#define UI_BN_CLICKED  139222333

// BN_HILITE    
#define UI_BN_HOVER  152811331

// BN_UNHILITE
#define UI_BN_LEAVE  152811332  

}

#endif // IBUTTON_H_787AE538_A2A3_4f45_A366_7AD02BC2EF9A