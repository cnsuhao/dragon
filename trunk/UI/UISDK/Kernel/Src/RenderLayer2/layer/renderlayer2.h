#pragma once
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"

//
// 2014.3.7 用于实现支持控件级别的分层设计。
// 原有的render layer只支持窗口级别的，而且只支持控件层的鼠标点击
// 
// Q1: 子renderlayer，是直接提交到窗口上，还是提交到上一层？
//     提交到上一层，用于做到设置一个层的opacity
//
namespace UI
{
    class  Object;
    class  WindowRender;
    class  WindowBase;
	class  RenderLayer2Soft3D;

// RENDER_LAYER2_STYLE

// 该层的内容是否需要重新渲染 
#define  RENDER_LAYER2_DIRTY    0x02   

// 禁止本层的updatelayer操作。由于调用UpdateObject会刷新自己，在作特效的时候自己并不需要刷新，则可将加上该标志
#define  RENDER_LAYER2_DISABLE_UPDATELAYER  0x04  


	interface IRenderLayerFilter
	{

	};
    
    // 层变换接口
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

        Object*         m_pObjCreate;    // 哪个对象创建了这个layer
        WindowRender*   m_pWindowRender;
        IRenderTarget*  m_pRenderTarget;

        // 为了实现3D光栅化线性插值时，需要一个哨兵边界，
        // 层的数据可能不是直接位于缓存的0，0处
        CRect   m_rcDrawInBuffer;  // 与RenderContext的m_ptBufferOffset对应

    public:
        IRenderLayerTransform*  m_pTransform;
        ROTATE_CENTER_TYPE      m_eRotateCenterType;
        POINT                   m_ptRotateCenterOffset;
    };


}
