#pragma once
#include "UISDK\Control\Inc\Interface\iscrollbar.h"
#include "UISDK\Kernel\Inc\Interface\iscrollbarmanager.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"

namespace UI
{
interface  IButton;

#define MIN_TRACK_BTN_SIZE 20

#define ALT_MSG_ID_BUTTON1       1
#define ALT_MSG_ID_BUTTON2       2
#define ALT_MSG_ID_THUMB_BTN     3

/*
<SystemVScrollBar id="__vscrollbar" width="18" bkgnd.render.type="theme.vscrollbar.bkg">
  <Button id="__scrollbar_up" bkgnd.render.type="theme.vscrollbar.up" height="18"/>
  <Button id="__scrollbar_thumb" bkgnd.render.type="theme.vscrollbar.thumb"/>
  <Button id="__scrollbar_down" bkgnd.render.type="theme.vscrollbar.down" height="18"/>
</SystemVScrollBar>
<SystemHScrollBar id="__hscrollbar" height="18" bkgnd.render.type="theme.hscrollbar.bkg">
  <Button id="__scrollbar_left" bkgnd.render.type="theme.hscrollbar.left" width="18"/>
  <Button id="__scrollbar_thumb" bkgnd.render.type="theme.hscrollbar.thumb"/>
  <Button id="__scrollbar_right" bkgnd.render.type="theme.hscrollbar.right" width="18"/>
</SystemHScrollBar>
*/

// ����ϵͳ���͹�����������������ť + �м�һ��THUBM��ť
class SystemScrollBar : public MessageProxy
{
public:
	SystemScrollBar();
	virtual ~SystemScrollBar();

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)
		UIMSG_WM_TIMER(OnTimer)
        UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
        UIMSG_WM_LBUTTONDBLCLK(OnLButtonDown)
        UIMSG_WM_LBUTTONUP(OnLButtonUp)
        UIMSG_WM_FINALRELEASE(FinalRelease)
        UIMESSAGE_HANDLER_EX(UI_WM_SCROLLBAR_INFO_CHANGED, 
                OnScrollBarInfoChanged)
        UIMESSAGE_HANDLER_EX(UI_WM_SCROLLBAR_BINDOBJHOVER_CHANGED, 
                OnBindObjHoverChanged)
        MESSAGE_HANDLER_EX(UI_WM_STATECHANGED2, OnHoverChanged)
        UIMSG_WM_ANIMATE_TICK(OnAnimateTick)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMESSAGE_HANDLER_EX(UI_WM_SCROLLBAR_SETISCROLLBARMGR, 
                OnSetIScrollBarMgr)
		UIMSG_WM_INITIALIZE(OnInitialize)
		UIMSG_WM_FINALCONSTRUCT(FinalConstruct)

	UIALT_MSG_MAP(ALT_MSG_ID_BUTTON1)
		UIMSG_WM_LBUTTONDOWN(OnBtn1LButtonDown)	
		UIMSG_WM_LBUTTONUP(OnBtn1LButtonUp)
		UIMSG_WM_KILLFOCUS(OnBtn1KillFocus)
        MESSAGE_HANDLER_EX(UI_WM_STATECHANGED2, OnHoverChanged)

	UIALT_MSG_MAP(ALT_MSG_ID_BUTTON2)
		UIMSG_WM_LBUTTONDOWN(OnBtn2LButtonDown)	
		UIMSG_WM_LBUTTONUP(OnBtn2LButtonUp)
		UIMSG_WM_KILLFOCUS(OnBtn2KillFocus)
        UIMESSAGE_HANDLER_EX(UI_WM_STATECHANGED2, OnHoverChanged)

        UIALT_MSG_MAP(ALT_MSG_ID_THUMB_BTN)
        UIMESSAGE_HANDLER_EX(UI_WM_STATECHANGED2, OnHoverChanged)

	UIALT_MSG_MAP(UIALT_CALLLESS)
		UIMSG_WM_CREATEBYEDITOR(OnCreateByEditor)

	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(SystemScrollBar, IControl)

public:
    void  SetISystemScrollBar(ISystemScrollBar* p) 
    {
        m_pISystemScrollBar = p; SetIMessageProxy(static_cast<IMessage*>(p)); 
    }
    void  SetIScrollBarMgr(IScrollBarManager* p);

	void  CalcScrollBarVisible();
	void  GetDesiredSize(SIZE* pSize);
    void  SetAutoSetBindObjFocus(bool b);
    void  SetFloatOnClientRegion(bool b);
    void  SetKeepHodeNonClientRegion(bool b);
    
protected:
	HRESULT  FinalConstruct(IUIApplication* p);
    void  OnInitialize();
    void  FinalRelease();
	void  OnCreateByEditor(CREATEBYEDITORDATA* pData);
	void  OnPaint(IRenderTarget*);
	void  OnTimer(UINT_PTR nIDEvent, LPARAM lParam);
	void  OnBtn1LButtonDown(UINT nFlags, POINT point);
	void  OnBtn2LButtonDown(UINT nFlags, POINT point);
	void  OnBtn1LButtonUp(UINT nFlags, POINT point);
	void  OnBtn1KillFocus(IObject* pNewFocusObj);
	void  OnBtn2LButtonUp(UINT nFlags, POINT point);
	void  OnBtn2KillFocus(IObject* pNewFocusObj);
    void  OnLButtonDown(UINT nFlags, POINT point);
    void  OnLButtonUp(UINT nFlags, POINT point);

    LRESULT  OnScrollBarInfoChanged(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnHoverChanged(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnBindObjHoverChanged(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnSetIScrollBarMgr(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void  OnAnimateTick(int nCount, UI::IStoryboard** ppArray);
protected:
	IObject*  GetBindObject();
    void  TrySetBindObjFocus();
    void  CheckHoverChanged();
    bool  IsScrollbarHover();

    void  CreateHoverAnimate();
    void  DestroyHoverAnimate();

	bool  CalcThumbButtonPos(bool bNeedUpdateThumbButtonSize); 
	void  CalcChannelRect(CRect* prc);
	int   CalcThumbButtonSize();

    virtual SCROLLBAR_DIRECTION_TYPE  GetDirType() { return VSCROLLBAR; }

private:
	UINT_PTR  m_nTimer1IDToScroll;    // ��סlineup button��500ms��ʼ����
	UINT_PTR  m_nTimer2IDToScroll;    // ��סlinedown button��500ms��ʼ����
	UINT_PTR  m_nTimer1IDScrolling;   // ��סlineup button����������
	UINT_PTR  m_nTimer2IDScrolling;   // ��סlinedown button����������

protected:
    ISystemScrollBar*   m_pISystemScrollBar;
    IScrollBarManager*  m_pMgrScrollBar;

    IButton*  m_pBtnLineUpLeft;
    IButton*  m_pBtnLineDownRight;
    IButton*  m_pBtnThumb;

    bool  m_bAutoSetBindObjFocus;   // ���������ʱ�����ؼ����ý���

	// ���ڼ�����ݷ����ı�ʱ��ˢ��thumbbtn��λ��
	int  m_nOldPage;
	int  m_nOldRange;
	int  m_nOldPos;

    bool  m_bHoverVisible;  // �������ؼ��������hoverʱ�ɼ���mac���

    enum SCROLLBAR_REGION_TYPE
    {
        HOLD_ONLY_VISIBLE,  // ֻ�ڿɼ�ʱռ�÷ǿͻ���
        FLOAT_ON_CLIENT,    // ��ռ�÷ǿͻ���λ�ã�Ʈ���ڿͻ�����
        KEEP_HOLD,          // һֱռ�÷ǿͻ����������Լ��Ƿ�ɼ�
    };
    SCROLLBAR_REGION_TYPE  m_eScrollBarRegionType;


    // ����
    bool  m_bHover;       // �б�ؼ����������Ƿ�hover.press����������������

	int   m_nClickDiff;   // �����Thumb btn����ʱ��λ�ñ�ʶ
	bool  m_bTracking;    // �Ƿ�������קThumb btn����
};  


class SystemVScrollBar : public SystemScrollBar
{
public:
    SystemVScrollBar();

    UI_DECLARE_OBJECT(SystemVScrollBar, OBJ_CONTROL|CONTROL_VSCROLLBAR);
    UI_BEGIN_MSG_MAP
        UIMSG_WM_SIZE(OnSize)
        UIMSG_WM_CALCPARENTNONCLIENTRECT(OnCalcParentNonClientRect)
        UIMESSAGE_HANDLER_EX(UI_WM_SCROLLBAR_BINDOBJSIZE_CHANGED, OnBindObjSize)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
    
    UIALT_MSG_MAP(ALT_MSG_ID_THUMB_BTN)
        UIMSG_WM_LBUTTONDOWN(OnThumbBtnLButtonDown)	
        UIMSG_WM_LBUTTONUP(OnThumbBtnLButtonUp)	
        UIMSG_WM_MOUSEMOVE(OnThumbBtnMousemove)

   UI_END_MSG_MAP_CHAIN_PARENT(SystemScrollBar)

public:
    void  SetISystemVScrollBar(ISystemVScrollBar* p) { m_pISystemVScrollBar = p; }
    virtual SCROLLBAR_DIRECTION_TYPE  GetDirType() { return VSCROLLBAR; }

protected:
    void  OnSize(UINT nType, int cx, int cy);
    void  OnCalcParentNonClientRect(CRegion4* prcNonClientRegion);
    LRESULT  OnBindObjSize(UINT uMsg, WPARAM wParam, LPARAM lParam);

    int   WindowPoint2TrackPos(int nUIPos);
    void  OnThumbBtnLButtonDown(UINT nFlags, POINT point);
    void  OnThumbBtnLButtonUp(UINT nFlags, POINT point);
    void  OnThumbBtnMousemove(UINT nFlags, POINT point);

private:
    ISystemVScrollBar*  m_pISystemVScrollBar;
};
class SystemHScrollBar : public SystemScrollBar
{
public:
	SystemHScrollBar();
    UI_DECLARE_OBJECT(SystemHScrollBar, OBJ_CONTROL|CONTROL_HSCROLLBAR);

	UI_BEGIN_MSG_MAP
		UIMSG_WM_SIZE(OnSize)
		UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
		UIMSG_WM_LBUTTONUP(OnLButtonUp)
		UIMSG_WM_CALCPARENTNONCLIENTRECT(OnCalcParentNonClientRect)
        UIMESSAGE_HANDLER_EX(UI_WM_SCROLLBAR_BINDOBJSIZE_CHANGED, OnBindObjSize)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)

	UIALT_MSG_MAP(ALT_MSG_ID_THUMB_BTN)
		UIMSG_WM_LBUTTONDOWN(OnThumbBtnLButtonDown)	
		UIMSG_WM_LBUTTONUP(OnThumbBtnLButtonUp)	
		UIMSG_WM_MOUSEMOVE(OnThumbBtnMousemove)

	UI_END_MSG_MAP_CHAIN_PARENT(SystemScrollBar)

public:
    void  SetISystemHScrollBar(ISystemHScrollBar* p) { m_pISystemHScrollBar = p; }
    virtual SCROLLBAR_DIRECTION_TYPE  GetDirType() { return HSCROLLBAR; }

protected:
	void  OnSize(UINT nType, int cx, int cy);
	void  OnCalcParentNonClientRect(CRegion4* prcNonClientRegion);
	LRESULT  OnBindObjSize(UINT uMsg, WPARAM wParam, LPARAM lParam);

	int   WindowPoint2TrackPos(int nUIPos);

	void  OnThumbBtnLButtonDown(UINT nFlags, POINT point);
	void  OnThumbBtnLButtonUp(UINT nFlags, POINT point);
	void  OnThumbBtnMousemove(UINT nFlags, POINT point);

private:
    ISystemHScrollBar*  m_pISystemHScrollBar;
};

}
