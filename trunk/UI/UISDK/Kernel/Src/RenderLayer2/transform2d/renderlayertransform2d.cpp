#include "stdafx.h"
#include "renderlayertransform2d.h"
#include "UISDK\Kernel\Src\Animate\3dwrap\3dlib\3dlib.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"

namespace UI
{

RenderLayer2Transform2d::RenderLayer2Transform2d(RenderLayer2*  p)
{
    Init_Sin_Cos_Tables();

    m_pRenderLayer = p;
    m_rcInvalid.SetRectEmpty();
    memset(&m_xForm, 0, sizeof(m_xForm));
    m_bNeedUpdatePipeline = false;
}
RenderLayer2Transform2d::~RenderLayer2Transform2d()
{
    
}

void  RenderLayer2Transform2d::Rotate(int x, int y, int z)
{
    UIASSERT(0 == x && 0 == y);  // 不支持横向翻转和纵向翻转

    CRect  rc;
    m_pRenderLayer->GetRectDrawInBuffer(&rc);
    POINT ptCenter = {0};
    switch (m_pRenderLayer->m_eRotateCenterType)
    {
    case ROTATE_CENTER_TOPLEFT:
        {
            ptCenter.x = m_pRenderLayer->m_ptRotateCenterOffset.x;
            ptCenter.y = m_pRenderLayer->m_ptRotateCenterOffset.y;
        }
        break;

    case ROTATE_CENTER_CENTER:
        {
            ptCenter.x = rc.left+rc.Width()/2 + m_pRenderLayer->m_ptRotateCenterOffset.x;
            ptCenter.y = rc.top+rc.Height()/2 + m_pRenderLayer->m_ptRotateCenterOffset.y;  
        }
        break;
    }
    

    float cosAngle = Fast_Cos((float)z);
    float sinAngle = Fast_Sin((float)z);

    m_xForm.eM11 = cosAngle;
    m_xForm.eM12 = sinAngle;
    m_xForm.eM21 = -sinAngle;
    m_xForm.eM22 = cosAngle;
    m_xForm.eDx = ptCenter.x - cosAngle*ptCenter.x + sinAngle*ptCenter.y;
    m_xForm.eDy = ptCenter.y - cosAngle*ptCenter.y - sinAngle*ptCenter.x;
}

void  RenderLayer2Transform2d::GetBufferPadding(RECT* prc)
{
    SetRectEmpty(prc);
}

// 这里需要重新设置剪裁区域，因为对象旋转，会超出当前剪裁区域
bool  RenderLayer2Transform2d::Commit(IRenderTarget*  pRenderTarget, Render2TargetParam* pParam)
{
	Object*  pObjLayer = m_pRenderLayer->GetCreateObject();
	Object* pParnt = pObjLayer->GetParentObject();
	UIASSERT(pParnt);

	RECT  rcClip;
	pParnt->GetVisibleClientRectInLayer(&rcClip);

	HRGN hRgn = CreateRectRgnIndirect(&rcClip);
	pRenderTarget->SelectClipRgn(hRgn);
	SAFE_DELETE_GDIOBJECT(hRgn);

    pParam->pTransform = &m_xForm;
	m_pRenderLayer->GetRenderTarget()->Render2Target(pRenderTarget, pParam);
    return true;
}

// x' = x * eM11 + y * eM21 + eDx, 
// y' = x * eM12 + y * eM22 + eDy, 
void RenderLayer2Transform2d::TransformPoint( POINT* pptLayer, __out POINT* pptTransform )
{
    pptTransform->x = round(pptLayer->x * m_xForm.eM11 + pptLayer->y * m_xForm.eM21 + m_xForm.eDx);
    pptTransform->y = round(pptLayer->x * m_xForm.eM12 + pptLayer->y * m_xForm.eM22 + m_xForm.eDy);
}

void  RenderLayer2Transform2d::GetRelativeBoundRect(RECT* prcInLayer, RECT* prcBound)
{
	CopyRect(prcBound, prcInLayer);
}

}