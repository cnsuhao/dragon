#ifndef BUTTON_H_FD4B3D3C_D26B_4e80_B1BB_5070F9E02316
#define BUTTON_H_FD4B3D3C_D26B_4e80_B1BB_5070F9E02316

#include "UISDK\Control\Inc\Interface\ibutton.h"
#define BUTTON_VK_SPACE_MOUSECAPTURE_NOTIFY_ID  138162231
namespace UI
{

//
//    ��ť�ؼ��ĳ�����࣬��CONTROL�Ļ������ٷ�װ�˰�״̬�����麯����CLICK�¼���Ӧ
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
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ButtonBase, IControl)   // �ɵ�ǰ��Ϣ����ཫ��Ϣ���ݸ�IControl����

    void SetIButtonBase(IButtonBase* p) { m_pIButtonBase = p; SetIMessageProxy(m_pIButtonBase); }

	// ��Ϣ����
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
	void  OnClicked(POINT* pt);         // �Լ�������ˣ������������������õ�ǰcheck״̬��

public:
	// �ⲿ�ӿ�
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
	bool  m_bSpaceKeyDown;     // �Ƿ����˿ո��
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

    BUTTON_AUTO_SIZE_TYPE  m_eAutoSizeType;             // ��ť�Զ�������С����
    ALIGN_TYPE  m_eIconAlignFlag;                       // ͼ�����ı���λ�ù�ϵ
    BUTTON_RENDER_DRAW_FOCUS_TYPE  m_eDrawFocusType;    // FOCUS����
    long  m_lIconMarginText;                             // ͼ������ı��Ĵ�С. ps: icon���밴ť��Ե�ľ����ͨ��padding��ʵ��
};

} 

#endif