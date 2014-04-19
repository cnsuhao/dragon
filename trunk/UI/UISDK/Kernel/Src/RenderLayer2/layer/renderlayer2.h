#pragma once
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"

//
// 2014.3.7 ����ʵ��֧�ֿؼ�����ķֲ���ơ�
// ԭ�е�render layerֻ֧�ִ��ڼ���ģ�����ֻ֧�ֿؼ���������
// 
// Q1: ��renderlayer����ֱ���ύ�������ϣ������ύ����һ�㣿
//     �ύ����һ�㣬������������һ�����opacity
//
namespace UI
{
    class  Object;
    class  WindowRender;
    class  WindowBase;
	class  RenderLayer2Soft3D;

// RENDER_LAYER2_STYLE

// �ò�������Ƿ���Ҫ������Ⱦ 
#define  RENDER_LAYER2_DIRTY    0x02   

// ��ֹ�����updatelayer���������ڵ���UpdateObject��ˢ���Լ���������Ч��ʱ���Լ�������Ҫˢ�£���ɽ����ϸñ�־
#define  RENDER_LAYER2_DISABLE_UPDATELAYER  0x04  


	interface IRenderLayerFilter
	{

	};
    
    // ��任�ӿ�
    enum RenderLayerTransformType
    {
        Transform_2d,
        Transform_soft3d,
        Trnasform_hard3d,
    };
    interface IRenderLayerTransform
    {
        virtual  ~IRenderLayerTransform() = 0 {};
        virtual  RenderLayerTransformType  GetTransformType() = 0;
        virtual  bool  Commit(IRenderTarget*  pRenderTarget, Render2TargetParam* pParam) = 0;
        virtual  void  GetBufferPadding(RECT* prc) = 0;
        virtual  void  OnSize(unsigned int nWidth, unsigned int nHeight) = 0;
        virtual  void  Rotate(int x, int y, int z) = 0;
        virtual  void  Move(int x, int y, int z) = 0;
        virtual  void  TransformPoint(POINT* pptLayer, __out POINT* pptTransform) = 0;
        virtual  void  GetRelativeBoundRect(RECT* prcInLayer, RECT* prcBound) = 0;
        virtual  void  GetInvalidRect(RECT* prc) = 0;
        virtual  bool  HitTest(POINT*  pptParent, POINT*  ptInChild) = 0;
    };

    class RenderLayer2
    {
    public:
        RenderLayer2(Object* pObj);
        virtual ~RenderLayer2();
		
		IRenderLayer2*  GetIRenderLayer2();

    public:
        HDC    GetHDC();
        bool   IsDirty();
        void   SetDirty(bool b);
        void   EnableUpdateLayer(bool b);
        bool   CanUpdateLayer();
        bool   IsTransparent();
        byte   GetOpacity();
		void   SetOpacity(byte b);

        Object*  GetCreateObject();
        IRenderTarget*  GetRenderTarget();
		RenderLayer2*  GetParentRenderLayer();

        void  OnSize(unsigned int nWidth, unsigned int nHeight);
        void  UpdateLayer(const RECT*  rcDamage);
        void  UpdateLayer(const RECT*  rcDamageArray, int nCount);

        bool  UpdateDescendant(Object*  pObjDamage, const RECT* prcObjDamageArray, int nCount, RECT* prcOutDamageInLayerArray);
    
        void  DirectCommit(bool bUpdate);
		void  Commit(IRenderTarget*  pRenderTarget);

        void  SetRotateCenterPos(ROTATE_CENTER_TYPE eType, int xOffset, int yOffset);
		void  Rotate3D(int x, int y, int z);
        void  Rotate2D(int degree);
        void  GetRectDrawInBuffer(RECT* prc);
        void  GetOffsetDrawInBuffer(POINT* ppt);

        bool  IsTransformed();
        bool  TransformPoint(POINT* pptInLayer, POINT* pptTransform);

    protected:
		IRenderLayer2*  m_pIRenderLayer2;
        long  m_lFlags;
		byte  m_nOpcity;

        Object*         m_pObjCreate;    // �ĸ����󴴽������layer
        WindowRender*   m_pWindowRender;
        IRenderTarget*  m_pRenderTarget;

        // Ϊ��ʵ��3D��դ�����Բ�ֵʱ����Ҫһ���ڱ��߽磬
        // ������ݿ��ܲ���ֱ��λ�ڻ����0��0��
        CRect   m_rcDrawInBuffer;  // ��RenderContext��m_ptBufferOffset��Ӧ

    public:
        IRenderLayerTransform*  m_pTransform;
        ROTATE_CENTER_TYPE      m_eRotateCenterType;
        POINT                   m_ptRotateCenterOffset;
    };


}
