#pragma once

namespace UI
{
    
// #define RENDERCONTEXT_FLAG_ERASEBKGND   0x0001    // �Ƿ���Ʊ���
// #define RENDERCONTEXT_FLAG_DRAWNCCHILD  0x0002    // �Ƿ���Ʒǿͻ����ӿؼ�
// #define RENDERCONTEXT_FLAG_PAINT        0x0004    // �Ƿ��������
// #define RENDERCONTEXT_FLAG_DRAWCHILD    0x0008    // �Ƿ�����ӿؼ�
// #define RENDERCONTEXT_FLAG_DRAWFULL  (RENDERCONTEXT_FLAG_ERASEBKGND|RENDERCONTEXT_FLAG_DRAWNCCHILD|RENDERCONTEXT_FLAG_PAINT|RENDERCONTEXT_FLAG_DRAWCHILD)

#define RENDERCONTEXT_FLAG_UPDATEOBJECT_DONOT_DRAWSELF  0x0010  // ����ʵ��UpdateObjectBkgnd,ˢ�±���

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
	bool   m_bUpdateClip;    // �Ƿ���Ҫά��CLIP����
    RECT   m_rcCurrentClip;  // ��¼��ǰ���ڻ��ƵĿؼ������ֻ꣬�е�m_bUpdateClipΪtrueʱ���ŻὫm_rcCurrentClip��InitClip�Ľ���ѡ��rendertarget
    POINT  m_ptOffset;       // ��¼��ǰ��HDCƫ���� �������ڹ���ʱ��m_ptOffset != m_rcCurrentClip.topleft��
	
    // ֻ�����ڻ����е�ƫ��������rendercontext�У��ؼ��Ը�ƫ���޸�֪������һ�����⣬���Ǵ���Commit����RenderLayer2::Commit��
    POINT  m_ptBufferOffset; // 2014.3.26 ���Ӳ������ڻ����е�ƫ�ơ����ϵ����ݲ�һ����ֱ�Ӿ���Ⱦ�������(0,0)λ�á���RenderLayer2::m_rcDrawInBuffer��Ӧ

private:
    unsigned int  m_nClipRectCount;  // ֧��һ��ˢ�¶����������listctrl
    RECT*   m_prcInitClip;   // ���ó�ʼ�������򣨼��Լ�Ҫˢ�µ����򣩡������������ߵ��������ߵ��Լ�ʱ����m_bUpdateClip����Ϊtrue�����������ڻ����Լ����Լ����Ӷ���ʱ�����Ӷ���
    long    m_lFlags;        // δʹ��
};

}

