#pragma once
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"
#include "UISDK\Project\UICompositor\Inc\ihardwarecompositor.h"

//
// 2014.3.7 ����ʵ��֧�ֿؼ�����ķֲ���ơ�
// ԭ�е�render layerֻ֧�ִ��ڼ���ģ�����ֻ֧�ֿؼ���������
// 
// Q1: ��renderlayer����ֱ���ύ�������ϣ������ύ����һ�㣿
// A1: �ύ����һ�㣬������������һ�����opacity
//
// Q1: RenderLayerʲôʱ����Ҫ���ػ棿
// A1: 1. ��ʼ��ʱ
//     2. Resize
//     3. �����Զ����µĻ��Ʒ�ʽʱ(��������UpdateLayer�����簴ť״̬�����ı䣬��ˢ�´��ڣ���ʱ��ť���ڲ�Ҳ��Ҫ���¡�
//

// 2014.11.22
// ����renderlayer������renderlayer�Ͻ�������ӳ�ˢ�µ����⡣
// ���磺Object::SetEnable  GetRenderLayer2()->SetDirty(true); 
// ����һ��update����ؼ�ʱ�����ؼ����ڲ�ˢ���������һ��ˢ��ʱ��
// ��һ�㷢�������dirty�ģ��ᴥ����һ���ȫ��ˢ�¡�
//
// �������dirty״̬�ݹ鴫�ݵ���renderlayer��Ҳ����һ�����⣬������ˢ��
// ��renderlayerҲ�����ᱻˢ��
//
//  
// 
namespace UI
{
class  Object;
class  WindowRender;
class  WindowBase;
class  RenderLayer2Soft3D;

// RENDER_LAYER2_STYLE

// - 2014.11.22������ʹ��һ����Ч��������ʶ��
// ԭ��1. ����һ���ؼ�������ˢ�£���ʱ��Ҫ��������Ϊdirty������һ�δ���ˢ�±��������layerʱ���������render
//       ��dirty�ģ���Ϊ������layer����ˢ�¡�����ʵֻ��Ҫˢ��ԭ�ؼ��������򼴿ɡ�
//
//       2. �޸�ĳ���ؼ������ԣ���������ˢ�£��ÿؼ����ڵĲ㻹�Ǹɾ��ģ���һ��ˢ�´���ʱ���ò�����ݲ���ȥ����
//
//       ��˽���һ��bool��������ʶlayer dirty�ǲ����ģ�����ֺܶ����⣬���Է�����
//
// �ò�������Ƿ���Ҫ������Ⱦ 
// #define  RENDER_LAYER2_DIRTY    0x02  


// �ñ�־������ʱ�������ϣ�1. ����ʱ  2. Resize�󣬲㻺�汻����
// ��ʾ��������Ҫȫ��ˢ��
#define RENDERLAYER2_FLAG_NEED_UPDATE  0x02

// ��ֹ�����updatelayer���������ڵ���UpdateObject��ˢ���Լ���������Ч��ʱ���Լ�������Ҫˢ�£���ɽ����ϸñ�־
#define  RENDERLAYER2_DISABLE_UPDATELAYER  0x04  

// ��ʾ����㲻�����������ݣ�ÿ��ˢ��ʱ�����»��ƣ�ֻ����֧�ֲ��һЩ���ԣ���֧��alpha����
// ����hover visible��scrollbar
#define RENDERLAYER_FLAG_ALWRAYS_UPDATE  0x0008

#if 0
interface IRenderLayerTransform
{
    virtual  ~IRenderLayerTransform() = 0 {};
    virtual  TRANSFORM_TYPE  GetTransformType() = 0;
	// ���ֲ������ύ��parent layer
    virtual  bool  Commit(IRenderTarget*  pRenderTarget, Render2TargetParam* pParam) = 0;
    virtual  void  GetBufferPadding(RECT* prc) = 0;
    virtual  void  OnSize(unsigned int nWidth, unsigned int nHeight) = 0;
    virtual  void  Rotate(float x, float y, float z) = 0;
    virtual  void  Move(int x, int y, int z) = 0;
	// ��λ�ڱ任ͼ���е�����ӳ�䵽δ�任ǰ�����꣬���ڵ������
    virtual  void  NormalPoint(POINT* pptLayer, __out POINT* pptTransform) = 0;
	// ���ڽ���������б任��������µ���Ч�����ύ��������
    virtual  void  GetRelativeBoundRect(RECT* prcInLayer, RECT* prcBound) = 0;
    virtual  void  GetInvalidRect(RECT* prc) = 0;
	// ��������ԣ��ڲ�������NormalPoint
    virtual  bool  HitTest(POINT*  pptParent, POINT*  ptInChild) = 0;
};
#endif

class RenderLayer
{
public:
    RenderLayer(Object* pObj);
    virtual ~RenderLayer();
	
	IRenderLayer*  GetIRenderLayer2();

public:
//     bool   IsDirty();
//     void   SetDirty(bool b);
	bool  NeedUpdate();
	void  SetNeedUpdate(bool b);
    void  SetAlwaysUpdate(bool b);

	void  InvalidateFull();
	void  InvalidateRect(RECT* prc);
	void  ClearInvalidateRegions();
	void  Refresh();

    void  EnableUpdateLayer(bool b);
    bool  CanUpdateLayer();
    bool  IsTransparent();
    byte  GetOpacity();
	void  SetOpacity(byte b);

    Object*  GetCreateObject();
    IRenderTarget*  GetRenderTarget();
	RenderLayer*  GetParentRenderLayer();
    IGpuLayerTexture*  GetGpuTexture();

	void  CreateTransform(TRANSFORM_TYPE eType);
	bool  HasTransform();
	IRenderLayerTransform2D*  GetTransform2D(bool bForceCreate=false);
	IRenderLayerTransform3D*  GetTransform3D(bool bForceCreate=false);
	IRenderLayerTransform2*   GetTransformPtr();
	
    void  OnSize(uint nWidth, uint nHeight);
    void  UpdateLayer(LPCRECT  rcDamage);
    void  UpdateLayer(LPCRECT  rcDamageArray, int nCount);

    bool  UpdateDescendantObject(Object*  pObjDamage, LPCRECT prcObjDamageArray, int nCount, RECT* prcOutDamageInLayerArray);

    void  DirectCommit(bool bUpdate);
	void  Commit(IRenderTarget*  pRenderTarget);

    void  GetRectDrawInBuffer(RECT* prc);
    void  GetOffsetDrawInBuffer(POINT* ppt);

    void  ClearChildren();
    void  AddChild(RenderLayer* pChild);
    void  Compositor2Gpu(GpuLayerCommitContext context);
	void  OnHardCompositeChanged(bool bEnable);
	bool  IsHardComposite();

protected:
	IRenderLayer*  m_pIRenderLayer2;
    long  m_lFlags;  
	byte  m_nOpcity;

    Object*         m_pObjCreate;    // �ĸ����󴴽������layer
    WindowRender*   m_pWindowRender;
    IRenderTarget*  m_pRenderTarget;


    // Ϊ��ʵ��3D��դ�����Բ�ֵʱ����Ҫһ���ڱ��߽磬
    // ������ݿ��ܲ���ֱ��λ�ڻ����0��0��
    CRect   m_rcDrawInBuffer;  // ��RenderContext��m_ptBufferOffset��Ӧ

	IRenderLayerTransform2*  m_pTransform2;

public:
    ROTATE_CENTER_TYPE      m_eRotateCenterType;
    POINT                   m_ptRotateCenterOffset;  // anchorPoint

	// Ӳ���ϳ�
    IGpuLayerTexture*  m_pGpuTexture;

    typedef vector<RenderLayer*>::iterator _ChildIter;
    vector<RenderLayer*>  m_vecChild;
};

}
