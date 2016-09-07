#ifndef BUTTON_H_FD4B3D3C_D26B_4e80_B1BB_5070F9E02316
#define BUTTON_H_FD4B3D3C_D26B_4e80_B1BB_5070F9E02316

#include "UISDK\Control\Inc\Interface\ibutton.h"
#define BUTTON_VK_SPACE_MOUSECAPTURE_NOTIFY_ID  138162231
namespace UI
{

//
//    按钮控件的抽象基类，在CONTROL的基础上再封装了按状态绘制虚函数、CLICK事件响应
//
class ButtonBase : public MessageProxy
{
public:
	ButtonBase();

	UI_DECLARE_OBJECT(ButtonBase, OBJ_CONTROL)

	UI_BEGIN_MSG_MAP
		UIMSG_WM_STATECHANGED2( OnStateChanged2 )
		UIMSG_WM_ERASEBKGND( OnEraseBkgnd )

		UIMSG_WM_LBUTTONDOWN( OnLButtonDown )
		UIMSG_WM_LBUTTONDBLCLK( OnLButtonDblClk )
		UIMSG_WM_LBUTTONUP( OnLButtonUp )
		UIMSG_WM_RBUTTONUP( OnRButtonUp )
		UIMSG_WM_KEYDOWN( OnKeyDown )
		UIMSG_WM_KEYUP( OnKeyUp )
		UIMSG_WM_KILLFOCUS( OnKillFocus )
		UIMSG_WM_SETFOCUS( OnSetFocus )
        UIMSG_WM_QUERYINTERFACE( QueryInterface )
    UIALT_MSG_MAP( BUTTON_VK_SPACE_MOUSECAPTURE_NOTIFY_ID )
		UIMESSAGE_HANDLER_EX(WM_ENTERSIZEMOVE, OnEnterSizeMove)
		else
        {
            return TRUE;
        }
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ButtonBase, IControl)   // 由当前消息入口类将消息传递给IControl处理

    void SetIButtonBase(IButtonBase* p) { m_pIButtonBase = p; SetIMessageProxy(m_pIButtonBase); }

	// 消息处理
protected:
	void  OnStateChanged2(UINT nMask);
	void  OnEraseBkgnd(IRenderTarget*);
// 	void  OnMouseMove(UINT nFlags, POINT point);
// 	void  OnMouseLeave();
 	void  OnLButtonDown(UINT nFlags, POINT point);
	void  OnLButtonUp(UINT nFlags, POINT point);
	void  OnRButtonUp(UINT nFlags, POINT point);
	void  OnLButtonDblClk(UINT nFlags, POINT point);
	void  OnSetFocus(IObject*);
	void  OnKillFocus(IObject*);
	void  OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags );
	void  OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags );
	LRESULT  OnEnterSizeMove(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void  DrawFocus(IRenderTarget*);
	void  OnClicked(POINT* pt);         // 自己被点击了（可在子类中用于设置当前check状态）

public:
	// 外部接口
	void  SetCheck(int nCheckState);
    void  SetCheck(bool bCheck, bool bUpdate);
	void  SetChecked();
	void  SetUnChecked();
	int   GetCheck();
	bool  IsChecked();
	long  GetButtonType();
	void  SetButtonType(long n);

protected:
    IButtonBase* m_pIButtonBase;
	long  m_lButtonType;
	int   m_nCheckState;       // BST_UNCHECKED, BST_CHECKED
	bool  m_bSpaceKeyDown;     // 是否按下了空格键
};


class Button : public ButtonBase
{
public:
    Button();
    ~Button();

    UI_DECLARE_OBJECT(Button, OBJ_CONTROL|CONTROL_BUTTON);

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_BN_CLICKED2(m_pIButton, OnClicked)
        UIMSG_WM_NOTIFY(UI_BN_DRAWFOCUS, DrawFocus)
        UIMSG_WM_GETDLGCODE(OnGetDlgCode)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_INITIALIZE(OnInitialize)
        UIMSG_WM_SERIALIZE(OnSerialize)
	UIALT_MSG_MAP(UIALT_CALLLESS)
		UIMSG_WM_CREATEBYEDITOR(OnCreateByEditor)
    UI_END_MSG_MAP_CHAIN_PARENT(ButtonBase)

public:
    void  SetIButton(IButton* p) { m_pIButton = p; }
    IButton*  GetIButton() { return m_pIButton; }

    void  SetTextAndUpdate(LPCTSTR  szText);
    void  SetText(LPCTSTR  szText);
    LPCTSTR  GetText();
	LPCTSTR  SaveText();

    void  SetIconFromFile(LPCTSTR szIconPath);
    void  SetIconFromHBITMAP(HBITMAP hBitmap);
    void  SetIconFromImageId(LPCTSTR szImageId);

    void  SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE eType);
    void  SetAutoSizeType(BUTTON_AUTO_SIZE_TYPE eType);
    void  SetDefaultPushButtonStyle(bool);
    bool  HasDefaultPushButtonStyle();

    ITextRenderBase*  GetTextRender();

protected:
    void  OnSerialize(SERIALIZEDATA*);
	void  OnCreateByEditor(CREATEBYEDITORDATA*);
    void  OnInitialize();
    void  OnClicked();

	void  GetDesiredSize(SIZE* pSize);
    UINT  OnGetDlgCode(LPMSG lpMsg);
    void  OnPaint(IRenderTarget*);
    LRESULT  DrawFocus(WPARAM, LPARAM);

private:
    void  analyzeIconTextRect(IRenderTarget*, CRect& rcIcon, CRect& rcText);

protected:
    IButton*  m_pIButton;
    String    m_strText;

    BUTTON_AUTO_SIZE_TYPE  m_eAutoSizeType;             // 按钮自动调整大小类型
    ALIGN_TYPE  m_eIconAlignFlag;                       // 图标与文本的位置关系
    BUTTON_RENDER_DRAW_FOCUS_TYPE  m_eDrawFocusType;    // FOCUS类型
    long  m_lIconMarginText;                             // 图标距离文本的大小. ps: icon距离按钮边缘的距离可通过padding来实现
};

} 

#endif