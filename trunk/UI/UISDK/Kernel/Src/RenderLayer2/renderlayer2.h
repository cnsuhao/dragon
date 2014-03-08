#pragma once
//
// 2014.3.7 用于实现支持控件级别的分层设计。
// 原有的render layer只支持窗口级别的。
//
namespace UI
{
    class  Object;

// RENDER_LAYER2_STYLE
#define  RENDER_LAYER2_HASALPHA  0x01   // 是bitblt提交，还是alphablend提交内容
#define  RENDER_LAYER2_DIRTY     0x02   // 该层的内容是否需要重新渲染 

    class RenderLayer2
    {
    public:
        RenderLayer2();
        ~RenderLayer2();

    public:
        bool  IsDirty();
        void  SetDirty(bool b);

    protected:
        long  m_lFlags;

        Object*  m_pObjCreate;    // 哪个对象创建了这个layer

        RenderLayer2*  m_pParentLayer;
        RenderLayer2*  m_pPrevLayer;
        RenderLayer2*  m_pNextLayer;
        RenderLayer2*  m_pFirstChildLayer;

        // 子renderlayer的z序列表
        // 先画小于0的，再画自己的object，最后画大于0的
        list<RenderLayer2*>  m_negZOrderList;
        list<RenderLayer2*>  m_posZOrderList;
    };

    // 窗口默认的控件层
    class WindowDefaultLayer : public RenderLayer2
    {
    public:

    };
}
