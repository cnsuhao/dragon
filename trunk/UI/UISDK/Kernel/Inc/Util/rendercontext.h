#pragma once

namespace UI
{
class UISDKAPI RenderContext
{
public:
	RenderContext(RECT* prcRenderRegion, bool bClip, bool bRequireAlphaChannel);

    void  DrawClient(IObject* pObject);
	void  Scroll(IObject* pObjScroll);
    bool  DrawChild(IObject* pChild);
	bool  DrawListItem(IListItemBase* pItem);
	
	void  Update(IRenderTarget* pRenderTarget);
	void  Reset(IRenderTarget* pRenderTarget);

public:
	bool      m_bUpdateClip;   // 是否需要维护CLIP参数
    bool      m_bRequireAlphaChannel;  // 是否需要alpha，用于分层窗口和areo窗口

	RECT      m_rcDrawRegion;  // 窗口坐标
	POINT     m_ptOffset;      // 当前要绘制的对象偏移量，其实就是设备坐标
};

}

