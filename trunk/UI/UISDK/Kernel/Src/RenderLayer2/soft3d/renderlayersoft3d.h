#pragma once
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\3dpipeline.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\texture_mapping_base.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\renderlayer2.h"

// ���ʵ�ֲ��ϵ�������ת����

namespace UI
{

//////////////////////////////////////////////////////////////////////////
//
// Q1: �ֲ�ˢ���Լ���ʱ������ṩ�Լ���������
//   UpdateObjectEx + Quad bound rect ??
//
// Q2: ��α���ÿ��ˢ�¶�Ҫ��quad������texture��û�б仯��ʱ��Ӧ�ò��ü���ȫ������Ϣ
//   ����һ��m_bNeedUpdatePipeline
// 
// Q3: ��μ��ϼ�����������3D��Ⱦ�������������ķ�Χ
//   ����ӳ�����һ������rcClip���ڻ���ǰ�ȼ�����Ʒ�Χ
//

class  TextureMappingBase;

class RenderLayer2Soft3D : public IRenderLayerTransform
{
public:
    RenderLayer2Soft3D(RenderLayer2*  p);
    ~RenderLayer2Soft3D();

    virtual  RenderLayerTransformType  GetTransformType()
    {
        return Transform_soft3d;
    }

    virtual  bool  Commit(IRenderTarget*  pRenderTarget, Render2TargetParam* pParam);
    virtual  void  GetBufferPadding(RECT* prc);
    virtual  void  OnSize(unsigned int nWidth, unsigned int nHeight);
    virtual  void  Rotate(int x, int y, int z);
    virtual  void  Move(int x, int y, int z);
    virtual  void  TransformPoint(POINT* pptLayer, __out POINT* pptTransform);
    virtual  void  GetRelativeBoundRect(RECT* prcInLayer, RECT* prcBound);
    virtual  void  GetInvalidRect(RECT* prc);
    virtual  bool  HitTest(POINT*  pptParent, POINT*  ptInChild);

protected:
    void  Update3dPos();

protected:
    RenderLayer2*  m_pRenderLayer;
    bool  m_bNeedUpdatePipeline;  // ֻ����Ҫ��ʱ�����3D����
    CRect  m_rcInvalid;

    TextureMappingBase*  m_pTexturemapping;

    // ��ǰ����ϵ�������ؼ��е����ת�����Լ�������ϵ
    MATRIX_4_4  m_matWorld;			
};

}