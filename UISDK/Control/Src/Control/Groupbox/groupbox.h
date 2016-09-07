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
    long      m_nTextIndent;      // 文字缩进。例如文字显示在左上角时， m_nTextIndent为文字距离左侧的大小
	long      m_nTextAlign;       // 文字对齐方式。如DT_LEFT|DT_TOP表示左上
    CRegion4  m_rcTextMargin;     // 文字与边框的距离。例如文字显示在左上角时，m_rcTextMargin.left + m_nTextIndent为文字距离左侧的大小

    SIZE      m_sizeText;         // 文本的大小
    CRect     m_rcBorderForImage; // bkgnd(边框)绘制区域，主要是减去了文字的高度的一半
    CRect     m_rcTextClipInWindow;  // 用于剪裁bkgnd render，不要穿过文字
    CRect     m_rcText;           // 文本绘制位置
    bool      m_bNeedCalcRect;    // 当控件大小改变后，需要重新计算位置
    bool      m_bNeedMeasureText; // 当改变文字内容后，需要重新计算文本长度和位置
};
}

#endif // GROUPBOX_H_FCD15471_6B71_4633_B7B5_DE5578707524