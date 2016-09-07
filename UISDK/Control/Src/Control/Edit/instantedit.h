#pragma once
#include "edit.h"

/*
    ��ʱ�༭��
    
    ����ͨedit������
    1. hoverʱ����������������û����һ��
    2. �Ǳ༭״̬�£��ı��ɲ���edit���ƣ�����instantedit���ƣ���֧����ʾ...
    3. �༭״̬�£��س�����ʧ���ᴥ��acccept notify
    4. �༭״̬�£�Esc���ᴥ��cancel notify

    ʹ�ó�����
    1. �б�ؼ����б��������޸�
    2. im����е���������
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