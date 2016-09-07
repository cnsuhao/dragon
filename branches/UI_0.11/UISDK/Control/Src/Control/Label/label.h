#pragma  once
#include "UISDK\Control\Inc\Interface\ilabel.h"

namespace UI
{
//
//  ���ֿؼ�
//
class Label : public MessageProxy
{
public:
	Label();
	~Label();

	UI_DECLARE_OBJECT(Label, OBJ_CONTROL|CONTROL_LABEL)

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_LBUTTONUP(OnLButtonUp)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_SERIALIZE(OnSerialize)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
    UIALT_MSG_MAP(UIALT_CALLLESS)
        UIMSG_WM_CREATEBYEDITOR(OnCreateByEditor)
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(Label, IControl)

    void  SetILabel(ILabel* p) { m_pILabel = p; SetIMessageProxy(static_cast<IMessage*>(p)); }

public:
    LPCTSTR  GetText();
    void  SetText(LPCTSTR pszText);

	// ��Ϣ����
protected:
	void  OnPaint( IRenderTarget* hDC );
	void  OnSerialize(SERIALIZEDATA*);
	void  GetDesiredSize(SIZE* pSize);
    void  OnLButtonUp(UINT nFlags, POINT point);
    HRESULT  FinalConstruct(IUIApplication* p);
    void  OnCreateByEditor(CREATEBYEDITORDATA* pData);

    void  OnClicked(POINT* pt);

private:
    ITextRenderBase*  GetTextRender();

	// ����
protected:
    ILabel*  m_pILabel;
	String  m_strText;
};


//
//	��̬ͼƬ�ؼ�(֧��ico��... )
//
class PictureCtrl : public MessageProxy
{
public:
	PictureCtrl();
	~PictureCtrl();

	UI_DECLARE_OBJECT(PictureCtrl, OBJ_CONTROL|CONTROL_PICTURE)

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
		UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
    UIALT_MSG_MAP(UIALT_CALLLESS)
        UIMSG_WM_CREATEBYEDITOR(OnCreateByEditor)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(PictureCtrl, IControl)

    void  SetIPictureCtrl(IPictureCtrl* p) { m_pIPictureCtrl = p; SetIMessageProxy(static_cast<IMessage*>(p)); }

private:
	void  GetDesiredSize(SIZE* pSize);
	void  OnPaint(IRenderTarget* pRenderTarget);
	HRESULT  FinalConstruct(IUIApplication* p);
    void  OnCreateByEditor(CREATEBYEDITORDATA* pData);

protected:
    IPictureCtrl*  m_pIPictureCtrl;
};


interface IGifImageRender;
//
//	����ͼƬ
//
//	TODO: ����PNG���ж�������
//
//  ע:
//      1. Ϊ�˱���gif�����ص��������Ȼ���߼�ʱ��ˢ�£���Start���������ⲿ����
//
class GifCtrl : public MessageProxy
{
public:
	GifCtrl();
	~GifCtrl();

	UI_DECLARE_OBJECT(GifCtrl, OBJ_CONTROL|CONTROL_GIFCTRL)

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)
		UIMSG_WM_MOVE(OnMove)
        UIMSG_WM_GETDESIREDSIZE(OnGetDesiredSize)
		UIMSG_WM_GIFFRAME_TICK(OnGifFrameTick)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(GifCtrl, IControl)

    void  SetIGifCtrl(IGifCtrl* p) { m_pIGifCtrl = p; SetIMessageProxy(static_cast<IMessage*>(p)); }
    IGifCtrl*  GetIGifCtrl() { return m_pIGifCtrl; }

public:
    bool  Start();
    bool  Pause();
    bool  Stop();

public:
	// virtual ����
	void  OnGetDesiredSize(SIZE* pSize);
	void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
	HRESULT  FinalConstruct(IUIApplication* p);

	void  OnPaint(IRenderTarget* pRenderTarget);
	void  OnMove(CPoint ptPos);
	void  OnGifFrameTick(WPARAM wParam, LPARAM lParam);

private:
    IGifCtrl*  m_pIGifCtrl;

	IGifImageRender*  m_pGifRender;
};
}