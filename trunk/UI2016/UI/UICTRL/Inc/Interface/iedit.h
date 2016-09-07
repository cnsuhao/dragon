#ifndef _UI_IEDIT_H_
#define _UI_IEDIT_H_
#include "..\..\..\UISDK\Inc\Interface\icontrol.h"

namespace UI
{
const UINT  EDIT_TEXTREDNER_STATE_NORMAL    = TEXTRENDER_STATE_NORMAL|0;
const UINT  EDIT_TEXTRENDER_STATE_SELECTED  = TEXTRENDER_STATE_SELECTED|1;

const UINT  EDIT_BKGND_RENDER_STATE_NORMAL  = RENDER_STATE_NORMAL|0;
const UINT  EDIT_BKGND_RENDER_STATE_HOVER   = RENDER_STATE_HOVER|1;
const UINT  EDIT_BKGND_RENDER_STATE_PRESS   = RENDER_STATE_PRESS|2;
const UINT  EDIT_BKGND_RENDER_STATE_DISABLE = RENDER_STATE_DISABLE|3;

// 在编辑框中按了下某个键
// wParam: RETURN/ESC/TAB
// return: 1已处理，0未处理
#define UI_EN_KEYDOWN  136181700

// 在编辑框中的文本发生了变化
// message: UI_WM_NOTIFY
// code:    UI_EN_CHANGE
// wParam:  0表示由用户输入触发，1表示由调用api(settext等)触发 bInvokeBySetText
#define UI_EN_CHANGE   136221429


struct EDITSTYLE
{
	bool  combobox : 1;       // 组合框中使用的编辑框
	bool  want_tab : 1;       // 接收TAB键消息
	bool  read_only : 1;      // 只读

};

class Edit;
interface __declspec(uuid("9EFA8C1A-21A6-415A-9C6C-C759ACAD2966"))
UICTRLAPI IEdit : public IControl
{
    void  SetText(LPCTSTR szText);
    void  SetTextLong(long lNumber);
	void  SetTextNoFilter(LPCTSTR szText);
    LPCTSTR  GetText();
    void  Clear();

    void  SetSel(int nStartChar, int nEndChar);
    void  GetSel(int& nStartChar,int& nEndChar) const;

    bool  IsReadOnly();
    void  SetReadOnly(bool b);
	
	void  SetTextFilterDigit();
	void  ClearTextFilter();

	UI_DECLARE_INTERFACE_ACROSSMODULE(Edit);
};

enum ACCEPT_EDIT_RESULT
{
	ACCEPT_EDIT_ACCEPT,  // 接受
	ACCEPT_EDIT_REJECT,  // 不接受
	ACCEPT_EDIT_REJECT_AND_CONTINUE_EDIT,  // 不接受，继续编辑
};

enum START_EDIT_RESULT
{
	START_EDIT_OK = 0,     // 允许
	START_EDIT_CANCEL = 1, // 取消，不允许编辑这项
};

// return: START_EDIT_RESULT
#define UI_EN_STARTEDIT  148042202
// return: ACCEPT_EDIT_RESULT
#define UI_EN_ACCEPTEDIT 148042203
#define UI_EN_CANCELEDIT 148042204
#define UI_EN_ENDEDIT    148042205

class InstantEdit;
interface __declspec(uuid("227EB0DD-D4EC-4A3A-83ED-25FEA28E123E"))
UICTRLAPI IInstantEdit : public IEdit
{
    UI_DECLARE_INTERFACE(InstantEdit);
};


// 当前大小写发生变发（只在有焦点时通知）
// wParam: IPasswordEdit
// lParam: 0/1 是否启用
// return: 1表示已处理，0表示未处理
#define UI_PASSWORDEDIT_NM_CAPSLOCK_CHANGED  151081409

// 输入密码统计
#define UI_PASSWORDEDIT_NM_INPUT_STATS  167200939





// 对象隐藏这个类，不导出
// 不向提供获取原始密码的接口，外部只能通过发消息获得
class PasswordEdit;
interface __declspec(uuid("5A7915E7-5F89-4660-A50C-CF9ED142AC4A"))
IPasswordEdit : public IEdit
{
	UI_DECLARE_INTERFACE(PasswordEdit);
};

// wParam: BSTR*
// lParam: bool* 
#define UI_PASSWORDEDIT_MSG_GETREALPASSWORD  151081651
inline void PasswordEdit_GetRealPassword(IMessage* pPasswordCtrl, void/*BSTR*/* pOut)
{
	UIASSERT(pPasswordCtrl && pOut);
	UISendMessage(pPasswordCtrl, UI_PASSWORDEDIT_MSG_GETREALPASSWORD, (WPARAM)pOut);
}

// wParam: LPCTSTR
#define UI_PASSWORDEDIT_MSG_SETREALPASSWORD  164152027
inline void PasswordEdit_SetRealPassword(IMessage* pPasswordCtrl, LPCTSTR pText)
{
	UIASSERT(pPasswordCtrl && pText);
	UISendMessage(pPasswordCtrl, UI_PASSWORDEDIT_MSG_SETREALPASSWORD, (WPARAM)pText);
}

// 原文、密码 模式切换
// wParam: PASSWORDEDIT_MODE
enum PASSWORDEDIT_MODE
{
	PASSWORDEDIT_MODE_PASSWORD,
	PASSWORDEDIT_MODE_PLAIN,
};
#define UI_PASSWORDEDIT_MSG_SWITCHMODE  169061750
inline void PasswordEdit_SwitchMode(IMessage* pPasswordCtrl, PASSWORDEDIT_MODE eMode)
{
	UIASSERT(pPasswordCtrl);
	UISendMessage(pPasswordCtrl, UI_PASSWORDEDIT_MSG_SWITCHMODE, (WPARAM)eMode);
}

}

#endif // _UI_IEDIT_H_