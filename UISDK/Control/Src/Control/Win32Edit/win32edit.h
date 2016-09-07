#pragma once
#ifdef COMPILE_Win32Edit
#include "UISDK\Control\Inc\Interface\iwin32edit.h"
#include "EditWrap.h"

namespace UI
{
class EditWrap;

class Win32Edit : public MessageProxy
{
public:
    Win32Edit();
    ~Win32Edit();

	UI_DECLARE_OBJECT_INFO(Win32Edit, OBJ_CONTROL|CONTROL_EDIT)

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
		UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
        MESSAGE_HANDLER_EX(UI_WM_STATECHANGED2, OnStateChanged)
        MSG_WM_LBUTTONDOWN(OnLButtonDown)
        MESSAGE_HANDLER_EX(UI_WM_INITIALIZE, OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(Win32Edit, IControl)

    void  SetIWin32Edit(IWin32Edit* p){
        m_pIWin32Edit = p;
        SetIMessageProxy(static_cast<IMessage*>(p));
    }
    IWin32Edit*  GetIWin32Edit() {
        return m_pIWin32Edit;
    }

public:
    void  SetText(LPCTSTR szText, bool bUpdateNow);
    void  SetPromptText(LPCTSTR szText, bool bUpdateNow);
    EditWrap*  StartEdit(bool bSelAll);
    void  DiscardEditNow();
    void  SetNotifyStartEdit(bool b);
    void  SetNotifyAcceptEdit(bool b);
    void  SetNotifyCancelEdit(bool b);
    void  SetNotifyDestroyEdit(bool b);
    bool  IsEditing();

    ACCEPT_EDIT_RESULT  OnEditWindowPreDestroy(HWND hWndEdit, EditDestroyFlag eFlag);
    void  OnEditWindowDestroy();

protected:    
    LRESULT  OnCreateProp(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnPropChanged(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnInitialize(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnStateChanged(UINT, WPARAM wParam, LPARAM);

    void  OnPaint(IRenderTarget* pRenderTarget);
	void  OnEraseBkgnd(IRenderTarget* pRenderTarget);
    void  OnLButtonDown(UINT nFlags, CPoint point);


protected:
//     void  DrawBkgImage(HDC hDC, IImageSecProp* pImage, SkinRect& rc);
//     void  DrawText(HDC hDC, IColorProp* pColor, SkinRect rc);
    void  DrawPrompt(HDC hDC, CRect rc);

protected:
    IWin32Edit*  m_pIWin32Edit;
    EditWrap*    m_pEditWindow;

    String   m_strText;
    String   m_strPrompt;
    bool     m_bShowEditWnd;  // 当前是否显示了编辑框窗口
    bool     m_bNotifyStartEdit;
    bool     m_bNotifyAcceptEdit;
    bool     m_bNotifyCancelEdit;
    bool     m_bNotifyDestroyEdit;

    HFONT    m_hFontEditWindow;  // dui字体转换为gdi字体

    bool     m_bShowPrompt;
    long  m_lLimitText;  // 0不限制
    bool  m_bByteLimit;  // 是否按字节进行限制
};

}
#endif