#pragma once
#include "edit.h"

/*
    即时编辑器
    
    与普通edit的区别：
    1. hover时会高亮，用于提醒用户点击一下
    2. 非编辑状态下，文本由不是edit绘制，而是instantedit绘制，并支持显示...
    3. 编辑状态下，回车键、失焦会触发acccept notify
    4. 编辑状态下，Esc键会触发cancel notify

    使用场景：
    1. 列表控件中列表项名称修改
    2. im软件中的心情短语等
*/

namespace UI
{

class InstantEdit :  public Edit, public IPreTranslateMessage
{
public:    
    InstantEdit();
    ~InstantEdit();

    UI_DECLARE_OBJECT(InstantEdit, OBJ_CONTROL|CONTROL_EDIT);
    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        MSG_WM_KEYDOWN(OnKeyDown)
        MESSAGE_HANDLER_EX(WM_LBUTTONDOWN, OnLButtonDown)
        MESSAGE_HANDLER_EX(WM_LBUTTONDBLCLK, OnLButtonDown)
        MESSAGE_HANDLER_EX(WM_RBUTTONDOWN, OnRButtonDown)
        MESSAGE_HANDLER_EX(WM_SETCURSOR, OnSetCursor)
        MSG_WM_KILLFOCUS(OnKillFocus)
        MSG_WM_SETFOCUS(OnSetFocus)
    UI_END_MSG_MAP_CHAIN_PARENT(Edit)

    void  SetIInstantEdit(IInstantEdit* p){
        m_pIInstantEdit = p;
    }

private:
    LRESULT  OnSetCursor(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void  OnSetFocus(HWND wndFocus);
    void  OnKillFocus(HWND wndFocus);
    void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
private:
	bool  beginEdit();
	void  cancelEdit();
	void  acceptEdit();
	void  endEdit();

private:
    IInstantEdit*  m_pIInstantEdit;

    bool  m_bEditing;
	String  m_strTextBeforeEdit;
};


}