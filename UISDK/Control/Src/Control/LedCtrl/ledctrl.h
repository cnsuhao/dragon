#pragma  once
#include "UISDK\Control\Inc\Interface\iledctrl.h"
#include "UISDK\Kernel\Inc\Interface\iimagerender.h"
namespace UI
{
//
//  默认每一项的宽度、高度都一样
//

class LEDCtrl : public MessageProxy
{
public:
	LEDCtrl();

    UI_DECLARE_OBJECT(LEDCtrl, OBJ_CONTROL|CONTROL_LEDCTRL)

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)	
        UIMSG_WM_LBUTTONUP(OnLButtonUp)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(LEDCtrl, IControl)

    void  SetILEDCtrl(ILEDCtrl* p) 
    {
        m_pILEDCtrl = p;
        SetIMessageProxy(static_cast<IMessage*>(p)); 
    }

public:
    void  SetIndexMap(LPCTSTR szText);
    void  SetText(LPCTSTR szText, bool bUpdate=true);

protected:
    HRESULT  FinalConstruct(IUIApplication* p);
	void  OnPaint(IRenderTarget*);
	void  SetAttribute(IMapAttribute* pMapAttr, bool bReload);
	void  GetDesiredSize(SIZE* pSize);
    void  OnLButtonUp(UINT nFlags, POINT point);

    void  OnClicked(POINT* pt);

protected:
    ILEDCtrl*  m_pILEDCtrl;

    // 该处直接借用m_foregndRender。
    // 因此要配置文件中应该将foregnd render配置为image list 列表
	IImageListRender*  m_pImagelist; 

    // 字符串到图片索引的映射
	map<TCHAR,int>    m_mapIndex;    

    // 每一项的显示宽度和宽度，默认为imagelistitem
    // 的width/height，但也支持属性配置
	SIZE       m_sizeItem;           

	vector<int>       m_vecIndex;    // 当前要显示的索引序列
	String            m_strText; 
};


}