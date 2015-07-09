#ifndef IEDIT_H_FF306559_C3C2_4f1a_BE33_588947FA8421
#define IEDIT_H_FF306559_C3C2_4f1a_BE33_588947FA8421

#include "UISDK\Kernel\Inc\Interface\icontrol.h"

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


class Edit;
interface UICTRLAPI IEdit : public IControl
{
    UI_DECLARE_Ixxx_INTERFACE(IEdit, Edit);

    void  SetText(LPCTSTR szText, bool bUpdate);
    void  SetTextLong(long lNumber, bool bUpdate);
	void  SetTextNoFilter(LPCTSTR szText, bool bUpdate);
    LPCTSTR  GetText();
    void  Clear(bool bUpdate);

    void  SetSel(int nStartChar, int nEndChar);
    void  GetSel(int& nStartChar,int& nEndChar) const;

    bool  IsReadOnly();
    void  SetReadOnly(bool b, bool bUpdate);
	
	void  SetTextFilterDigit();
	void  ClearTextFilter();
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
interface UICTRLAPI IInstantEdit : public IEdit
{
    UI_DECLARE_Ixxx_INTERFACE(IInstantEdit, InstantEdit);

};

}

#endif // IEDIT_H_FF306559_C3C2_4f1a_BE33_588947FA8421