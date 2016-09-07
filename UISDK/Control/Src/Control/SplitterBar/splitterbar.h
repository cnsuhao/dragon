#pragma once
#include "UISDK\Control\Inc\Interface\isplitterbar.h"

namespace UI
{
// 注：由于splitter要在自己加载完之后就获取相应控件，因此需要将splitterbar放在两个控件的后面

class SplitterBar : public MessageProxy
{
public:
    SplitterBar();
    ~SplitterBar();

    UI_DECLARE_OBJECT(SplitterBar, OBJ_PANEL|CONTROL_SPLITTERBAR);

public:
    enum{ PARENT_MSG_ALT_ID = 146142134 };

    UI_BEGIN_MSG_MAP
        UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
        UIMSG_WM_LBUTTONUP(OnLButtonUp)
        UIMSG_WM_MOUSEMOVE(OnMouseMove)
        UIMSG_WM_CANCELMODE(OnCancelMode)
        UIMSG_WM_SIZE(OnSize)

		UIMSG_WM_SERIALIZE(OnSerialize)
        UIMSG_WM_INITIALIZE(OnObjectLoaded)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
		UIMSG_WM_FINALRELEASE(OnFinalRelease)
		UIMSG_WM_FINALCONSTRUCT(OnFinalConstruct)
	UIALT_MSG_MAP(PARENT_MSG_ALT_ID)
		UIMSG_WM_SIZE(OnParentSize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(SplitterBar, IPanel);

    void  SetISplitterBar(ISplitterBar* p)
    {
        m_pISplitterBar = p;
        SetIMessageProxy(static_cast<IMessage*>(p));
    }
    ISplitterBar*  GetISplitterBar()
    {
        return m_pISplitterBar;
    }

public:
    void  SetObjectsPosType(SPLITTED_OBJECTS_POS eDir);
	SPLITTED_OBJECTS_POS  GetObjectsPosType();

    void  SetAlign(SPLITTERBAR_ALIGN eAligh);
	SPLITTERBAR_ALIGN  GetAlign();

    bool  IsTopBottom();
    bool  IsLeftRight();
    void  EnableDrag(bool b);
    bool  IsDragable(); 
    void  UpdateControlPosBySplitterBarCurPos(bool bRedraw);

    int   GetLeftMin();
    int   GetLeftMax();
    int   GetRightMin();
    int   GetRightMax();
    int   GetUpMin();
    int   GetUpMax();
    int   GetDownMin();
    int   GetDownMax();

protected:
	HRESULT  OnFinalConstruct(IUIApplication* p);
	void  OnFinalRelease();
    void  OnSerialize(SERIALIZEDATA* pData);
    void  OnObjectLoaded();
    void  OnSize(UINT nType, int cx, int cy);
	void  OnParentSize(UINT nType, int cx, int cy);

    void  OnLButtonDown(UINT nFlags, POINT point);
    void  OnLButtonUp(UINT nFlags, POINT point);
    void  OnCancelMode();
    void  OnMouseMove(UINT nFlags, POINT point);

    bool  OnDraging(POINT ptInParent);
    bool  TestAvailableDragPos(POINT point, POINT* ptAvailable);
    void  UpdateLeftRightCtrlPos(int nPos);
    void  UpdateUpDownCtrlPos(int nPos);
    void  OnParentSizeChanged();
    void  EndDrag();

    void  CalcAvailableRegion();

private:
	void  set_left_obj_id(LPCTSTR);
	void  set_right_obj_id(LPCTSTR);
	void  set_top_obj_id(LPCTSTR);
	void  set_bottom_obj_id(LPCTSTR);
	LPCTSTR  get_left_obj_id();
	LPCTSTR  get_right_obj_id();
	LPCTSTR  get_top_obj_id();
	LPCTSTR  get_bottom_obj_id();

private:
    ISplitterBar*   m_pISplitterBar;
    
    SPLITTED_OBJECTS_POS  m_eObjectsPos;   // 子对象排布的方向
    SPLITTERBAR_ALIGN     m_eAlignType;    // 父对象改变大小时，如何布局子对象

    union
    {
        struct
        {
			String  m_strLeftObjId;
			String  m_strRightObjId;

            IObject*  m_pObjLeft;     // 注：这两个对象必须配置在splitterbar前面，否则在splitterbar::onobjloaded中查找不到
            IObject*  m_pObjRight;    //     这两个对象必须与splitterbar位于同一个parent下面

            long  m_nLeftMin;
            long  m_nLeftMax;
            long  m_nLeftOffset;       // 控件与分隔条之间的间距偏移量。不一定非要对齐

            long  m_nRightMin;
            long  m_nRightMax;
            long  m_nRightOffset;
        };
        struct
        {
			String  m_strTopObjId;
			String  m_strBottomObjId;

            IObject*  m_pObjTop;            
            IObject*  m_pObjBottom;

            long  m_nTopMin;
            long  m_nTopMax;
            long  m_nTopOffset;

            long  m_nBottomMin;
            long  m_nBottomMax;
            long  m_nBottomOffset;
        };
    };
    IObject*  m_pObjParent;

    bool   m_bEnableDrag;
    bool   m_bButtonDown;
    int    m_nDeviation;      // 鼠标按下时的偏差
    POINT  m_ptLButtonDown;   // 窗口坐标
    POINT  m_ptLastMouseMove; // 窗口坐标

    CRect  m_rcAvailable;     // 在鼠标按下与弹起之间，该值有效
};

}