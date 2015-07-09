#ifndef GROUPBOX_H_FCD15471_6B71_4633_B7B5_DE5578707524
#define GROUPBOX_H_FCD15471_6B71_4633_B7B5_DE5578707524
#include "UISDK\Control\Inc\Interface\igroupbox.h"

namespace UI
{
class GroupBox : public MessageProxy
{
public:
	GroupBox();
	~GroupBox();

	UI_DECLARE_OBJECT(GroupBox, OBJ_PANEL|CONTROL_GROUPBOX);

	UI_BEGIN_MSG_MAP
		UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
        UIMSG_WM_SIZE(OnSize)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_SERIALIZE(OnSerialize)
		UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(GroupBox, IPanel)

    void  SetIGroupBox(IGroupBox* p) { 
		m_pIGroupBox = p; 
		SetIMessageProxy(static_cast<IMessage*>(p)); 
	}

private:
	HRESULT  FinalConstruct(IUIApplication* p);
	void  OnEraseBkgnd(IRenderTarget*);
    void  OnSize(UINT nType, int cx, int cy);
	void  OnSerialize(SERIALIZEDATA* pData);

    void  SetText(LPCTSTR szText);
	LPCTSTR  GetText();

private:
    void  MeasureText();
    void  CalcBorderRect();

protected:
    IGroupBox*  m_pIGroupBox;

	String    m_strText;
    long      m_nTextIndent;      // ��������������������ʾ�����Ͻ�ʱ�� m_nTextIndentΪ���־������Ĵ�С
	long      m_nTextAlign;       // ���ֶ��뷽ʽ����DT_LEFT|DT_TOP��ʾ����
    CRegion4  m_rcTextMargin;     // ������߿�ľ��롣����������ʾ�����Ͻ�ʱ��m_rcTextMargin.left + m_nTextIndentΪ���־������Ĵ�С

    SIZE      m_sizeText;         // �ı��Ĵ�С
    CRect     m_rcBorderForImage; // bkgnd(�߿�)����������Ҫ�Ǽ�ȥ�����ֵĸ߶ȵ�һ��
    CRect     m_rcTextClipInWindow;  // ���ڼ���bkgnd render����Ҫ��������
    CRect     m_rcText;           // �ı�����λ��
    bool      m_bNeedCalcRect;    // ���ؼ���С�ı����Ҫ���¼���λ��
    bool      m_bNeedMeasureText; // ���ı��������ݺ���Ҫ���¼����ı����Ⱥ�λ��
};
}

#endif // GROUPBOX_H_FCD15471_6B71_4633_B7B5_DE5578707524