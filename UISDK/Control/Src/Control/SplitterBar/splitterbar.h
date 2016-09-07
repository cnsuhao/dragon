#pragma once
#include "UISDK\Control\Inc\Interface\isplitterbar.h"

namespace UI
{
// ע������splitterҪ���Լ�������֮��ͻ�ȡ��Ӧ�ؼ��������Ҫ��splitterbar���������ؼ��ĺ���

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
    
    SPLITTED_OBJECTS_POS  m_eObjectsPos;   // �Ӷ����Ų��ķ���
    SPLITTERBAR_ALIGN     m_eAlignType;    // ������ı��Сʱ����β����Ӷ���

    union
    {
        struct
        {
			String  m_strLeftObjId;
			String  m_strRightObjId;

            IObject*  m_pObjLeft;     // ע���������������������splitterbarǰ�棬������splitterbar::onobjloaded�в��Ҳ���
            IObject*  m_pObjRight;    //     ���������������splitterbarλ��ͬһ��parent����

            long  m_nLeftMin;
            long  m_nLeftMax;
            long  m_nLeftOffset;       // �ؼ���ָ���֮��ļ��ƫ��������һ����Ҫ����

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
    int    m_nDeviation;      // ��갴��ʱ��ƫ��
    POINT  m_ptLButtonDown;   // ��������
    POINT  m_ptLastMouseMove; // ��������

    CRect  m_rcAvailable;     // ����갴���뵯��֮�䣬��ֵ��Ч
};

}