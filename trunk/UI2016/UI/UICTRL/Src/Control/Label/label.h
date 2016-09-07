#pragma  once
#include "Inc\Interface\ilabel.h"

namespace UI
{
//
//  文字控件
//
class Label : public MessageProxy, public UI::IBindSourceChangedListener
{
public:
	Label(ILabel* p);
	~Label();

	UI_BEGIN_MSG_MAP()
		UIMSG_PAINT(OnPaint)
        //MSG_WM_LBUTTONUP(OnLButtonUp)
        UIMSG_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_QUERYINTERFACE(Label)
        UIMSG_SERIALIZE(OnSerialize)
        UIMSG_FINALCONSTRUCT(FinalConstruct)
//     UIALT_MSG_MAP(UIALT_CALLLESS)
//         UIMSG_WM_CREATEBYEDITOR(OnCreateByEditor)
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(Label, IControl)

public:
    LPCTSTR  GetText();
    void  SetText(LPCTSTR pszText);
	void  SetTextAndInvalidate(LPCTSTR pszText);

	IBindSourceChangedListener*  GetListener() {
		return static_cast<IBindSourceChangedListener*>(this);
	}

	// 消息处理
protected:
	void  OnPaint( IRenderTarget* hDC );
	void  OnSerialize(SERIALIZEDATA*);
	void  GetDesiredSize(SIZE* pSize);
    //void  OnLButtonUp(UINT nFlags, POINT point);
    HRESULT  FinalConstruct(ISkinRes* p);
    //void  OnCreateByEditor(CREATEBYEDITORDATA* pData);

    //void  OnClicked(POINT* pt);

	virtual void OnBindSourceChanged(long key, bindable* src, void* context) override;

	// 属性
protected:
    ILabel*  m_pILabel;
	String  m_strText;
};


//
//	静态图片控件(支持ico、... )
//
class PictureCtrl : public MessageProxy
{
public:
	PictureCtrl(IPictureCtrl* p);
	~PictureCtrl();

	UI_BEGIN_MSG_MAP()
		UIMSG_PAINT(OnPaint)
		UIMSG_GETDESIREDSIZE(GetDesiredSize)
		UIMSG_QUERYINTERFACE(PictureCtrl)
		UIMSG_FINALCONSTRUCT(FinalConstruct)
//     UIALT_MSG_MAP(UIALT_CALLLESS)
//         UIMSG_WM_CREATEBYEDITOR(OnCreateByEditor)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(PictureCtrl, IControl)

private:
	void  GetDesiredSize(SIZE* pSize);
	void  OnPaint(IRenderTarget* pRenderTarget);
	HRESULT  FinalConstruct(ISkinRes* p);
    //void  OnCreateByEditor(CREATEBYEDITORDATA* pData);

protected:
    IPictureCtrl*  m_pIPictureCtrl;
};


interface IGifImageRender;
//
//	动画图片
//
//	TODO: 增加PNG序列动画类型
//
//  注:
//      1. 为了避免gif在隐藏的情况下仍然在走计时器刷新，将Start操作交给外部调用
//
class GifCtrl : public MessageProxy
{
public:
	GifCtrl(IGifCtrl* p);
	~GifCtrl();

	UI_BEGIN_MSG_MAP()
		UIMSG_PAINT(OnPaint)
		MSG_WM_MOVE(OnMove)
        UIMSG_GETDESIREDSIZE(OnGetDesiredSize)
		//UIMSG_WM_GIFFRAME_TICK(OnGifFrameTick)
        UIMSG_QUERYINTERFACE(GifCtrl)
        UIMSG_FINALCONSTRUCT(FinalConstruct)
        //UIMSG_WM_SETATTRIBUTE(SetAttribute)
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(GifCtrl, IControl)

    IGifCtrl*  GetIGifCtrl() { return m_pIGifCtrl; }

public:
    bool  Start();
    bool  Pause();
    bool  Stop();

public:
	// virtual 函数
	void  OnGetDesiredSize(SIZE* pSize);
	void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
	HRESULT  FinalConstruct(ISkinRes* p);

	void  OnPaint(IRenderTarget* pRenderTarget);
	void  OnMove(CPoint ptPos);
	void  OnGifFrameTick(WPARAM wParam, LPARAM lParam);

private:
    IGifCtrl*  m_pIGifCtrl;

	IGifImageRender*  m_pGifRender;
};
}