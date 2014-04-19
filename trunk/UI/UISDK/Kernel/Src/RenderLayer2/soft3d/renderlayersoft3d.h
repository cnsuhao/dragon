#pragma once
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\3dpipeline.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\texture_mapping_base.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\renderlayer2.h"

// 软件实现层上的数据旋转绘制

namespace UI
{

//////////////////////////////////////////////////////////////////////////
//
// Q1: 局部刷新自己的时候，如何提供自己的脏区域
//   UpdateObjectEx + Quad bound rect ??
//
// Q2: 如何避免每次刷新都要求quad和设置texture，没有变化的时候应该不用计算全部的信息
//   增加一个m_bNeedUpdatePipeline
// 
// Q3: 如何加上剪裁区，限制3D渲染不超出父容器的范围
//   纹理映射添加一个参数rcClip，在绘制前先计算绘制范围
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
    bool  m_bNeedUpdatePipeline;  // 只在需要的时候更新3D坐标
    CRect  m_rcInvalid;

    TextureMappingBase*  m_pTexturemapping;

    // 当前坐标系，将父控件中的鼠标转换到自己的坐标系
    MATRIX_4_4  m_matWorld;			
};

}