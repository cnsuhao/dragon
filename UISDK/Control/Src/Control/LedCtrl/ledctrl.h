#pragma  once
#include "UISDK\Control\Inc\Interface\iledctrl.h"
#include "UISDK\Kernel\Inc\Interface\iimagerender.h"
namespace UI
{
//
//  Ĭ��ÿһ��Ŀ�ȡ��߶ȶ�һ��
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

    // �ô�ֱ�ӽ���m_foregndRender��
    // ���Ҫ�����ļ���Ӧ�ý�foregnd render����Ϊimage list �б�
	IImageListRender*  m_pImagelist; 

    // �ַ�����ͼƬ������ӳ��
	map<TCHAR,int>    m_mapIndex;    

    // ÿһ�����ʾ��ȺͿ�ȣ�Ĭ��Ϊimagelistitem
    // ��width/height����Ҳ֧����������
	SIZE       m_sizeItem;           

	vector<int>       m_vecIndex;    // ��ǰҪ��ʾ����������
	String            m_strText; 
};


}