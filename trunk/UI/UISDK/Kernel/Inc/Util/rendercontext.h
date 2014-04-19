#pragma once

namespace UI
{
    
// #define RENDERCONTEXT_FLAG_ERASEBKGND   0x0001    // 是否绘制背景
// #define RENDERCONTEXT_FLAG_DRAWNCCHILD  0x0002    // 是否绘制非客户区子控件
// #define RENDERCONTEXT_FLAG_PAINT        0x0004    // 是否绘制内容
// #define RENDERCONTEXT_FLAG_DRAWCHILD    0x0008    // 是否绘制子控件
// #define RENDERCONTEXT_FLAG_DRAWFULL  (RENDERCONTEXT_FLAG_ERASEBKGND|RENDERCONTEXT_FLAG_DRAWNCCHILD|RENDERCONTEXT_FLAG_PAINT|RENDERCONTEXT_FLAG_DRAWCHILD)

#define RENDERCONTEXT_FLAG_UPDATEOBJECT_DONOT_DRAWSELF  0x0010  // 用于实现UpdateObjectBkgnd,刷新背景

class UISDKAPI RenderContext
{
public:
	RenderContext(const RECT*  prcInitClip, const RECT* prcCurrentDraw, bool bClip);
    RenderContext(const RECT*  prcInitClipArray, unsigned int nCount, const RECT* prcRenderRegion, bool bClip);
    RenderContext(const RenderContext& o);
    ~RenderContext();

    void  DrawClient(IObject* pObject);
    bool  DrawChild(IObject* pChild);
	bool  DrawListItem(IListItemBase* pItem);
	
	bool  Update(IRenderTarget* pRenderTarget);
    void  InitClip(IRenderTarget* pRenderTarget, bool bClear);

    // flags
    void  ModifyFlags(long lAdd, long lRemove);
    bool  TestFlags(long lFlag);

protected:
    HRGN  CreateCurrentClipRegion();

public:
	bool   m_bUpdateClip;    // 是否需要维护CLIP参数
    RECT   m_rcCurrentClip;  // 记录当前正在绘制的控件的坐标，只有当m_bUpdateClip为true时，才会将m_rcCurrentClip与InitClip的交集选入rendertarget
    POINT  m_ptOffset;       // 记录当前的HDC偏移量 （当存在滚动时，m_ptOffset != m_rcCurrentClip.topleft）
	
    // 只将层在缓存中的偏移体现在rendercontext中，控件对该偏移无感知。（有一个例外，就是窗口Commit、和RenderLayer2::Commit）
    POINT  m_ptBufferOffset; // 2014.3.26 增加层数据在缓存中的偏移。层上的内容不一定是直接就渲染到缓存的(0,0)位置。与RenderLayer2::m_rcDrawInBuffer对应

private:
    unsigned int  m_nClipRectCount;  // 支持一次刷新多个区域，例如listctrl
    RECT*   m_prcInitClip;   // 设置初始剪裁区域（即自己要刷新的区域）。当绘制流程走到从祖先走到自己时，将m_bUpdateClip设置为true，这样就能在绘制自己及自己的子对象时剪裁子对象
    long    m_lFlags;        // 未使用
};

}

