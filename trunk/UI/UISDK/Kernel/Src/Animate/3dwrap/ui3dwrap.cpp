#include "stdafx.h"
#include "ui3dwrap.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Src\UIEffect\CacheBitmap\cachebitmap.h"
#include "UISDK\Kernel\Inc\Interface\iobject3dwrap.h"
#include "UISDK\Kernel\Src\RenderLayer\3dlayer.h"
#include "UISDK\Kernel\Src\RenderLayer\renderchain.h"
#include <float.h>

namespace UI
{

Object3DWrap::Object3DWrap(Object* pObj)
{
    m_pObject = pObj;
    m_pIObject3DWrap = NULL;
    m_pOldParent = NULL;
    m_pOldParentMsg = NULL;
    m_p3dLayer = NULL;

    m_bBegin = false;
    m_bParamChanged = true;
    
#if 1
    m_pTexturemapping = new QuadPerspectiveTextureMapping;   // 支持了抗锯齿 + AlphaBlend
#elif 1
    m_pTexturemapping = new AffineTextureMapping;
#elif 1
    m_pTexturemapping = new PerspectiveTextureMapping;  // 支持了分段仿射映射
#endif

    if (!g_bInitSinCosTable)
    {
        g_bInitSinCosTable = true;

        // http://technet.microsoft.com/zh-cn/library/e9b52ceh(v=vs.110).aspx
        _control87(_RC_DOWN, _MCW_RC); //设置FPU的舍入模式，在Bilinear函数中需要使用汇编加快float->int。
        Build_Sin_Cos_Tables();
    }
}

Object3DWrap::~Object3DWrap()
{
//    End(); <-- 由object负责调用End
    SAFE_DELETE(m_pIObject3DWrap);
    SAFE_DELETE(m_pTexturemapping);
}

IObject3DWrap*  Object3DWrap::GetIObject3DWrap()
{
    if (!m_pIObject3DWrap)
        m_pIObject3DWrap = new IObject3DWrap(this);

    return m_pIObject3DWrap;
}


void  Object3DWrap::Move(int x, int y, int z)
{
    m_3dpipeline.m_nOffsetX = x;
    m_3dpipeline.m_nOffsetY = y;
    m_3dpipeline.m_nOffsetZ = z;
    m_bParamChanged = true;
    m_p3dLayer->SetDirty(true);
}
void  Object3DWrap::Rotate(int x, int y, int z)
{
    // 计算时用的居然是相反的，先在这里减去360度吧
    m_3dpipeline.m_nRotateX = 360-x;
    m_3dpipeline.m_nRotateY = 360-y;
    m_3dpipeline.m_nRotateZ = 360-z;
    m_bParamChanged = true;
    m_p3dLayer->SetDirty(true);
}

bool  Object3DWrap::IsRunning()
{
    return m_bBegin;
}
void  Object3DWrap::Begin()
{
    if (!m_pObject)
        return;

    if (m_bBegin)
        End();

    RenderChain*  pRenderChain = m_pObject->GetRenderChain();
    if (!pRenderChain)
        return;

    pRenderChain->On3dObjectBegin();  // 通知创建3d layer
    m_p3dLayer = pRenderChain->Get3DLayer();
    if (!m_p3dLayer)
        return;

    m_pOldParent = m_pObject->GetParentObject();
    if (m_pOldParent)
    {
        m_pOldParentMsg = m_pOldParent->GetIMessage();
        m_pObject->RemoveMeInTheTree();
        m_p3dLayer->AddChild(m_pObject);
    }
    m_bBegin = true;

    int nWidth = m_pObject->GetWidth();
    int nHeight = m_pObject->GetHeight();

    if (0 == nWidth || 0 == nHeight)
        return;

    m_pObject->AddHook(this->GetIMessage(), 0, 0);
    
    // 默认是对象中心作为原点
    CRect rc;
    m_pObject->GetWindowRect(&rc);

    m_3dpipeline.m_ptOrigin.x = rc.left + (rc.Width()>>1);
    m_3dpipeline.m_ptOrigin.y = rc.top + (rc.Height()>>1);
    
    CRect rcTexture(0, 0, rc.Width(), rc.Height());
    rcTexture.OffsetRect(1, 1);

    m_pTexturemapping->SetTexturePointA(rc.left, rc.top, rcTexture.left, rcTexture.top);
    m_pTexturemapping->SetTexturePointB(rc.right, rc.top, rcTexture.right, rcTexture.top);
    m_pTexturemapping->SetTexturePointC(rc.right, rc.bottom, rcTexture.right, rcTexture.bottom);
    m_pTexturemapping->SetTexturePointD(rc.left, rc.bottom, rcTexture.left, rcTexture.bottom);

	Image*  pDestBuffer = m_p3dLayer->GetMemoryLayerBuffer();
	if (!pDestBuffer || pDestBuffer->IsNull())
		return;

	m_pTexturemapping->SetDstBuffer(pDestBuffer);
}


void  Object3DWrap::End()
{
    if (!m_bBegin)
        return;

    if (!m_p3dLayer)
        return;

    if (m_pOldParent)
    {
        if (m_pObject->GetUIApplication()->IsUIObjectAvailable(m_pOldParentMsg))
        {
            m_pObject->RemoveMeInTheTree();
            if (m_pObject->IsNcObject())
            {
                m_pOldParent->AddNcChild(m_pObject);
            }
            else
            {
                m_pOldParent->AddChild(m_pObject);
            }
        }
        m_pOldParent = NULL;
        m_pOldParentMsg = NULL;
    }
    m_p3dLayer->GetRenderChain()->On3dObjectEnd();
    m_p3dLayer = NULL;

    m_pObject->RemoveHook(this->GetIMessage());
    m_bParamChanged = true;
}

// object自己被销毁了，此时就不用再做其它操作了(例如直接退出了程序，没有再调用End)
// 而且Object3DWrap自己下一步也将被释放
void  Object3DWrap::EndByDestroy()
{
    if (!m_bBegin)
        return;

    m_bBegin = false;

    // m_p3dLayer->GetRenderChain()->On3dObjectEnd();  会崩溃。例如当前正在释放3DLayer，会导致再次释放
    m_p3dLayer = NULL;
    m_pOldParent = NULL;
    m_pOldParentMsg = NULL;
}


// 计算对象的局部（模型）坐标 
// 以m_ptOrigin为坐标原点，也是世界坐标原点
void  Object3DWrap::SetOriginPoint(POINT pt)
{
    if (!m_pObject)
        return;

    m_3dpipeline.m_ptOrigin.x = pt.x;
    m_3dpipeline.m_ptOrigin.y = pt.y;
    m_bParamChanged = true;
    m_p3dLayer->SetDirty(true);
}

UINT  Object3DWrap::OnHitTest( POINT* pt )
{
//     CRect rcParent;
//     m_pObject->GetParentRect(&rcParent);
// 
//     BOOL bIn = ::PtInRect(&rcParent, *pt);
// 
//     if (!bIn)
//         return HTNOWHERE;
// 
//     POINT ptSelf = { pt->x - rcParent.left, pt->y - rcParent.top };
// 
//     float fx = 0.0f, fy = 0.0f;
//     perspective_transform_fp(m_perspective, ptSelf.x, ptSelf.y, &fx, &fy);
//     if (fx < 0 || fy < 0 || fx > m_bufferDst.GetWidth() || fy > m_bufferDst.GetHeight())
//         return HTNOWHERE;

    return HTCLIENT;
}

void  Object3DWrap::OnMouseMove(UINT nFlags, POINT point)
{
    // 将窗口坐标转移为对象坐标
//     POINT ptObj;
//     m_pObject->WindowPoint2ObjectPoint(&point, &ptObj);
// 
//     // 将坐标转换为变形前的坐标
//     float fx = 0.0f, fy = 0.0f;
//     perspective_transform_fp(m_perspective, ptObj.x, ptObj.y, &fx, &fy);
// 
//     // 将对象坐标转换为窗口坐标
//     POINT ptWnd = m_pObject->GetRealPosInWindow();
//     
//     UIMSG msg = *GetCurMsg();
//     msg.lParam = MAKELPARAM((int)(ptWnd.x+fx), (int)(ptWnd.y+fy));
//     m_pObject->GetIObject()->ProcessMessage(&msg, 0, false);
}

void  Object3DWrap::OnDrawObject(IRenderTarget* pRenderTarget, RenderContext roc)
{   
    int nObjWidth = m_pObject->GetWidth();
    int nObjHeight = m_pObject->GetHeight();
    if (0 == nObjWidth || 0 == nObjWidth)
        return;

    if (m_bParamChanged)
    {
        CRect rc;
        m_pObject->GetWindowRect(&rc);

        m_3dpipeline.update_locale_pos(&rc);
        m_3dpipeline.Do();

        m_pTexturemapping->SetQuad(&m_3dpipeline.m_quad, &rc);
        m_bParamChanged = false;
    }

    // 控件绘制出来的图，上下左右边框多加了1px，用于实现插值的边界处理
    int nBufferWidth = nObjWidth + 2;
    int nBufferHeight = nObjHeight + 2;
    Image*  pObjBuffer = CacheBitmap::GetInstance()->CreateEx(nBufferWidth, nBufferHeight);
    if (!pObjBuffer)
        return;

    IRenderTarget* pTempRenderTarget = UICreateRenderTarget(pRenderTarget->GetGraphicsRenderLibraryType());
    {
        HDC hDC = pObjBuffer->GetDC();
        SetBkMode(hDC, TRANSPARENT);
        SetStretchBltMode(hDC, HALFTONE);
        pTempRenderTarget->BindHDC(hDC);

        // 获取源数据
        pTempRenderTarget->BeginDraw(NULL, 0);
        {
            // 设置剪裁和偏移量（减掉边界值）
            CRect rc(0, 0, nBufferWidth, nBufferHeight);
            rc.DeflateRect(1,1,1,1);
            RenderContext rocTemp(&rc, true, roc.m_bRequireAlphaChannel);
            rocTemp.m_ptOffset.x = rocTemp.m_ptOffset.y = 1;
            rocTemp.Update(pTempRenderTarget);

            // 将控件绘制到临时缓存中
            m_pObject->RealDrawObject(pTempRenderTarget, rocTemp);
        }
        pTempRenderTarget->EndDraw();
        SAFE_RELEASE(pTempRenderTarget);
        pObjBuffer->ReleaseDC();
    }
	m_pTexturemapping->SetSrcBuffer(pObjBuffer);

    // 对源数据进行处理
    m_pTexturemapping->Do();
}


}
