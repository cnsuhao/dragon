#pragma once
#include "UISDK\Control\Inc\Interface\iprogressctrl.h"

namespace UI
{

//   ������
class ProgressBar : public MessageProxy
{
public:
	ProgressBar();

    UI_DECLARE_OBJECT(ProgressBar, OBJ_CONTROL|CONTROL_PROGRESSBAR)
    
    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ProgressBar, IControl)

protected:
    HRESULT  FinalConstruct(IUIApplication* );
    void  OnPaint(IRenderTarget* pDC);
    void  ResetAttribute();
    void  SetAttribute(IMapAttribute* pMapAttr, bool bReload);
    void  GetDesiredSize(SIZE* pSize);

public:
    void  SetIProgressBar(IProgressBar* p) { m_pIProgressBar = p; SetIMessageProxy(static_cast<IMessage*>(p)); }

	int   SetPos(int nPos, bool bUpdate=true);
	int   SetPage(int nPage, bool bUpdate=true);
	void  SetRange(int nLower, int nUpper, bool bUpdate=true);
	void  SetScrollInfo(LPCSCROLLINFO lpsi, bool bUpdate=true);

	void  SetLine(int nLine){ m_nLine = nLine; }
	int   GetPos(){ return m_nCur; }
	int   GetPage(){ return m_nPage; }
	int   GetLine(){ return m_nLine; }
	void  GetRange(int& nLower, int& nUpper){ nLower = m_nMin; nUpper = m_nMax; }
	int   GetRange() { return m_nMax - m_nMin; }

	void  SetDirectionType(PROGRESSBAR_SCROLL_DIRECTION_TYPE eType) { m_eDirectionType = eType; }
	PROGRESSBAR_SCROLL_DIRECTION_TYPE  GetDirectionType() { return m_eDirectionType; }

	// ����
protected:
    IProgressBar*  m_pIProgressBar;

	int  m_nMin;        // ��Сֵ
	int  m_nMax;        // ���ֵ
	int  m_nCur;        // ��ǰλ��
	int  m_nLine;       // ÿ�ΰ���ʱ�������ľ���
	int  m_nPage;       // һ��ҳ��Ĵ�С
	PROGRESSBAR_SCROLL_DIRECTION_TYPE   m_eDirectionType;       // ����������,����
};


}