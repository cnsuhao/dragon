#ifndef PANEL_H_A91AD9AE_6579_4060_90A1_4FAD6D27D94B
#define PANEL_H_A91AD9AE_6579_4060_90A1_4FAD6D27D94B

#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"

namespace UI
{

class Panel : public Object
{
public:
	Panel();
	~Panel();

	UI_DECLARE_OBJECT(Panel, OBJ_PANEL)

	UI_BEGIN_MSG_MAP
		UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
		UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_POSTPAINT(OnPostPaint)
        UIMSG_WM_GETDESIREDSIZE(OnGetDesiredSize)
		UIMESSAGE_HANDLER_EX(UI_WM_GETLAYOUT, OnGetLayoutPtr)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
		UIMSG_WM_SERIALIZE(OnSerialize)
	UI_END_MSG_MAP_CHAIN_PARENT(Object)

    IPanel* GetIPanel() { return m_pIPanel; }
    void    SetIPanel(IPanel* p) { m_pIPanel = p; }

public:
    ILayout*  GetLayout();
    LAYOUTTYPE  GetLayoutType();
    void  SetLayoutType(LAYOUTTYPE eLayoutType);

    void  SetTextureRender(IRenderBase* p);
    IRenderBase*  GetTextureRender();

protected:
	virtual void  virtualOnSize(UINT nType, UINT nWidth, UINT nHeight) override;

	void  OnEraseBkgnd(IRenderTarget*);
	void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnPostPaint(IRenderTarget* pRenderTarget);
	LRESULT  OnGetLayoutPtr(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void  OnSerialize(SERIALIZEDATA* pData);
	void  OnGetDesiredSize(SIZE* pSize);  // 命名为On,避免与object::GetDesiredSize冲突 

protected:
    void  LoadTextureRender(LPCTSTR szName) { loadRenderBase(szName, m_pTextureRender); }
    void  LoadMaskRender(LPCTSTR szName) { loadRenderBase(szName, m_pMaskRender); }
    LPCTSTR  GetTextureRenderName() { return getRenderBaseName(m_pTextureRender); }
    LPCTSTR  GetMaskRenderName() { return getRenderBaseName(m_pMaskRender); }

private:
    
protected:
    IPanel*    m_pIPanel;
	ILayout*   m_pLayout;
	CRegion4   m_rcBkgndRenderRegion;
	CRegion4   m_rcForegndRenderRegion;

    // 用于支持换肤功能的皮肤图片
    IRenderBase*   m_pTextureRender;
    CRegion4       m_rcTextureRenderRegion;

    // 最上面的遮罩层
    IRenderBase*   m_pMaskRender;  
    CRegion4       m_rcMaskRenderRegion;
};

}

#endif // PANEL_H_A91AD9AE_6579_4060_90A1_4FAD6D27D94B