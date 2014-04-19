#pragma once
#include "UISDK\Kernel\Src\RenderLayer2\layer\renderlayer2.h"

namespace UI
{
class RenderLayer2Transform2d : public IRenderLayerTransform
{
public:
    RenderLayer2Transform2d(RenderLayer2*  p);
    ~RenderLayer2Transform2d();

    virtual  RenderLayerTransformType  GetTransformType()
    {
        return Transform_2d;
    }

    virtual  bool  Commit(IRenderTarget*  pRenderTarget, Render2TargetParam* pParam);
    virtual  void  GetBufferPadding(RECT* prc);
    virtual  void  OnSize(unsigned int nWidth, unsigned int nHeight){};
    virtual  void  Rotate(int x, int y, int z);
    virtual  void  Move(int x, int y, int z){};
    virtual  void  TransformPoint(POINT* pptLayer, __out POINT* pptTransform);
    virtual  void  GetRelativeBoundRect(RECT* prcInLayer, RECT* prcBound);
    virtual  void  GetInvalidRect(RECT* prc){}
    virtual  bool  HitTest(POINT*  pptParent, POINT*  ptInChild){return true;};

protected:
    RenderLayer2*  m_pRenderLayer;
    bool   m_bNeedUpdatePipeline;  // 只在需要的时候更新3D坐标
    CRect  m_rcInvalid;

//  | eM11 eM12 0 | 
//  | eM21 eM22 0 | 
//  | eDx  eDy  1 | 
    XFORM  m_xForm;
};

}