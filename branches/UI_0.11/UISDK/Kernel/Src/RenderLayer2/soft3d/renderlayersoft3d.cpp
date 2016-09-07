#include "stdafx.h"
#include "renderlayersoft3d.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\quad_perspective_texture_mapping.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\affine_texture_mapping.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\perspective_texture_mapping.h"

namespace UI
{
#if 0
RenderLayer2Soft3D::RenderLayer2Soft3D(RenderLayer2*  p)
{
    m_pRenderLayer = p;
    MAT_IDENTITY_4_4(&m_matWorld);
    m_rcInvalid.SetRectEmpty();

#if 1
    m_pTexturemapping = new QuadPerspectiveTextureMapping;   // 支持了抗锯齿 + AlphaBlend
#elif 0
    m_pTexturemapping = new AffineTextureMapping;       // 没有再持续更新了
#elif 0
    m_pTexturemapping = new PerspectiveTextureMapping;  // 没有再持续更新了
#endif

    m_bNeedUpdatePipeline = false;
}
RenderLayer2Soft3D::~RenderLayer2Soft3D()
{
    SAFE_DELETE(m_pTexturemapping);
}

void  RenderLayer2Soft3D::Move(int x, int y, int z)
{
}
void  RenderLayer2Soft3D::Rotate(float x, float y, float z)
{
    // 计算时用的居然是相反的，先在这里减去360度吧
	MAT_IDENTITY_4_4(&m_matWorld);
    Mat_Rotate_4X4(&m_matWorld, 360.f-x, 360.f-y, 360.f-z);
    m_bNeedUpdatePipeline = true;
}

void  RenderLayer2Soft3D::GetInvalidRect(RECT* prc)
{
    Update3dPos();
    CopyRect(prc, &m_rcInvalid);
}

// 获取层内部一个区域（如子控件）在旋转之后的绑定区域，用于上一层刷新该控件脏区域
void  RenderLayer2Soft3D::GetRelativeBoundRect(RECT* prcInLayer, RECT* prcBound)
{
    // TODO: 有别的近似的办法来计算吗

    Object*  pLayerObj = m_pRenderLayer->GetCreateObject();
    CRect    rcLayer(0, 0, pLayerObj->GetWidth(), pLayerObj->GetHeight());

    POINT2D ptOrigin = {0}; // 坐标原点
    switch (m_pRenderLayer->m_eRotateCenterType)
    {
    case TRANSFORM_ROTATE_ORIGIN_LEFT:
        {
            ptOrigin.x = (float)m_pRenderLayer->m_ptRotateCenterOffset.x;
            ptOrigin.y = (float)m_pRenderLayer->m_ptRotateCenterOffset.y;
        }
        break;

    case TRANSFORM_ROTATE_ORIGIN_CENTER:
        {
            ptOrigin.x = rcLayer.Width()/2.0f + m_pRenderLayer->m_ptRotateCenterOffset.x;
            ptOrigin.y = rcLayer.Height()/2.0f + m_pRenderLayer->m_ptRotateCenterOffset.y;  
        }
        break;
    }

    Quad  quad;
    _3dPipeline  _3dpipeline;
    _3dpipeline.Do(prcInLayer, ptOrigin, &m_matWorld, __out &quad);

    quad.GetBoundRect2(prcBound);
}

void  RenderLayer2Soft3D::OnSize(unsigned int nWidth, unsigned int nHeight)
{
    m_bNeedUpdatePipeline = true;

    // 设置纹理坐标
    RECT  rcTexture;
    m_pRenderLayer->GetRectDrawInBuffer(&rcTexture);

    rcTexture.right--;  // 纹理索引不包括右侧和底侧
    rcTexture.bottom--;

    m_pTexturemapping->SetTexturePointA(rcTexture.left, rcTexture.top);
    m_pTexturemapping->SetTexturePointB(rcTexture.right, rcTexture.top);
    m_pTexturemapping->SetTexturePointC(rcTexture.right, rcTexture.bottom);
    m_pTexturemapping->SetTexturePointD(rcTexture.left, rcTexture.bottom);

    // 设置纹理(src)和目标缓存(dest)
    IRenderTarget*  pLayerRT = m_pRenderLayer->GetRenderTarget();

    ImageData  srcBuffer;
    pLayerRT->GetRenderBufferData(&srcBuffer);
    m_pTexturemapping->SetSrcBuffer(&srcBuffer);
}

void  RenderLayer2Soft3D::Update3dPos()
{
    if (!m_bNeedUpdatePipeline)
        return;
    m_bNeedUpdatePipeline = false;


    // 将原点设置为控件中心，以中心为原点进行旋转
    CRect  rc;
    m_pRenderLayer->GetRectDrawInBuffer(&rc);
    rc.right--;  //不包含右下侧
    rc.bottom--;

    POINT2D  ptOrigin = {rc.left + (rc.Width()/2.0f), rc.top + (rc.Height()/2.0f)};// 坐标原点

    Quad  quad;
    _3dPipeline  _3dpipeline;
    _3dpipeline.Do(&rc, ptOrigin, &m_matWorld, __out &quad);

    // 保存脏区域
    if (m_rcInvalid.IsRectEmpty())
    {
        m_pTexturemapping->m_quad.GetBoundRect2(&m_rcInvalid);
        m_rcInvalid.OffsetRect(-rc.left, -rc.top);
    }

    // 保存在m_pTexturemapping，到时取也从这里取吧
    // 设置纹理坐标
    m_pTexturemapping->SetQuad(&quad, &rc);
}

bool  RenderLayer2Soft3D::Commit(IRenderTarget*  pRenderTarget, Render2TargetParam* pParam)
{
    if (!pRenderTarget)
        return true;

    //  3d 坐标变换
    Update3dPos();

    // 设置目标缓存
    ImageData  dstBuffer;
    pRenderTarget->GetRenderBufferData(&dstBuffer);
    m_pTexturemapping->SetDstBuffer(&dstBuffer);

    // 位置
    POINT  ptOffset;
    pRenderTarget->GetViewportOrgEx(&ptOffset);

    POINT  ptOffsetBuffer;
    m_pRenderLayer->GetOffsetDrawInBuffer(&ptOffsetBuffer);
    ptOffset.x -= ptOffsetBuffer.x;
    ptOffset.y -= ptOffsetBuffer.y;

    // 父对象在层上的剪裁区域，3D旋转不能超出父容器所在区域
    Object*  pObjLayer = m_pRenderLayer->GetCreateObject();
    Object* pParnt = pObjLayer->GetParentObject();
    UIASSERT(pParnt);
    RECT  rcClip;
    pParnt->GetVisibleClientRectInLayer(&rcClip);

    // 渲染（抛弃rendercontext中的rcclip，直接使用父区域进行剪裁）
    m_pTexturemapping->Do(&ptOffset, &rcClip);

    m_rcInvalid.SetRectEmpty();
    return true;
}


void  RenderLayer2Soft3D::GetBufferPadding(RECT* prc)
{
    UIASSERT(m_pTexturemapping);
    m_pTexturemapping->GetTexturePadding(prc);
}

// 将层上的坐标（不考虑buffer offset）也做3D变换
void  RenderLayer2Soft3D::NormalPoint(POINT* pptLayer, __out POINT* pptNormal)
{
    POINT  ptInTexture = *pptLayer;

    POINT  ptBufferOffset;
    m_pRenderLayer->GetOffsetDrawInBuffer(&ptBufferOffset);

    ptInTexture.x += ptBufferOffset.x;
    ptInTexture.y += ptBufferOffset.y;

    float  fx = 0, fy = 0;
    m_pTexturemapping->MousePoint2TexturePoint(ptInTexture, &fx, &fy);

    pptNormal->x = round(fx) - ptBufferOffset.x;
    pptNormal->y = round(fy) - ptBufferOffset.y;
}

bool  RenderLayer2Soft3D::HitTest(POINT*  pptParent, POINT*  ptInChild)
{
    POINT  ptInTexture = *pptParent;

    UIASSERT(m_pTexturemapping);
    Object*  pObj = m_pRenderLayer->GetCreateObject();

    // 先判断是否在bound区域
    CRect  rcBound;
    m_pTexturemapping->m_quad.GetBoundRect2(&rcBound);

    CRect  rcOffsetInBuffer;
    m_pRenderLayer->GetRectDrawInBuffer(&rcOffsetInBuffer);

    ptInTexture.x += rcOffsetInBuffer.left;
    ptInTexture.y += rcOffsetInBuffer.top;
    ptInTexture.x -= pObj->GetParentRectL();
    ptInTexture.y -= pObj->GetParentRectT();

    if (!rcBound.PtInRect(ptInTexture))
        return false;

    float  fx = 0, fy = 0;
    m_pTexturemapping->MousePoint2TexturePoint(ptInTexture, &fx, &fy);

    float fLeft = (float)rcOffsetInBuffer.left;
    float fTop = (float)rcOffsetInBuffer.top;
    float fRight = (float)rcOffsetInBuffer.right;
    float fBottom = (float)rcOffsetInBuffer.bottom;

    if (fx < fLeft || fx > fRight)
        return false;
    if (fy < fTop || fy > fBottom)
        return false;

    if (ptInChild)
    {
        ptInChild->x = round(fx) - rcOffsetInBuffer.left;
        ptInChild->y = round(fy) - rcOffsetInBuffer.top;
    }

    return true;
}
#endif
}