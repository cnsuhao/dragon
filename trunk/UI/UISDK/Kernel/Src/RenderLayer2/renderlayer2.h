#pragma once
//
// 2014.3.7 ����ʵ��֧�ֿؼ�����ķֲ���ơ�
// ԭ�е�render layerֻ֧�ִ��ڼ���ġ�
//
namespace UI
{
    class  Object;

// RENDER_LAYER2_STYLE
#define  RENDER_LAYER2_HASALPHA  0x01   // ��bitblt�ύ������alphablend�ύ����
#define  RENDER_LAYER2_DIRTY     0x02   // �ò�������Ƿ���Ҫ������Ⱦ 

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

        Object*  m_pObjCreate;    // �ĸ����󴴽������layer

        RenderLayer2*  m_pParentLayer;
        RenderLayer2*  m_pPrevLayer;
        RenderLayer2*  m_pNextLayer;
        RenderLayer2*  m_pFirstChildLayer;

        // ��renderlayer��z���б�
        // �Ȼ�С��0�ģ��ٻ��Լ���object����󻭴���0��
        list<RenderLayer2*>  m_negZOrderList;
        list<RenderLayer2*>  m_posZOrderList;
    };

    // ����Ĭ�ϵĿؼ���
    class WindowDefaultLayer : public RenderLayer2
    {
    public:

    };
}
