#include "stdafx.h"
#include "software_layer.h"
#include "compositor.h"
#include "Inc/Interface/renderlibrary.h"


SoftwareLayer::SoftwareLayer()
{
    m_pRenderTarget = NULL;
}

SoftwareLayer::~SoftwareLayer()
{
    SAFE_RELEASE(m_pRenderTarget);
}

IRenderTarget*  SoftwareLayer::GetRenderTarget()
{
    if (!m_pRenderTarget)        
    {
        if (!m_pCompositor)
            return NULL;

        m_pCompositor->CreateRenderTarget(&m_pRenderTarget);
        if (!m_pRenderTarget)
            return NULL;

        m_pRenderTarget->CreateRenderBuffer(NULL);
    }

    return m_pRenderTarget;
}

void  SoftwareLayer::virtualOnSize(uint nWidth, uint nHeight)
{
    if (!m_pRenderTarget)
    {
        GetRenderTarget();
    }
    m_pRenderTarget->ResizeRenderBuffer(nWidth, nHeight);
}


void  SoftwareLayer::UpdateDirty()
{
    if (!m_pLayerContent)
        return;

    if (!m_dirtyRectangles.GetCount())
        return;

    IRenderTarget* pRenderTarget = GetRenderTarget();
    uint nCount = m_dirtyRectangles.GetCount();
    for (uint i = 0; i < nCount; i++)
        pRenderTarget->Clear(m_dirtyRectangles.GetRectPtrAt(i));

    pRenderTarget->BeginDraw();

    pRenderTarget->SetMetaClipRegion(
        m_dirtyRectangles.GetArrayPtr(), 
        m_dirtyRectangles.GetCount());

    m_pLayerContent->Draw(pRenderTarget);
    pRenderTarget->EndDraw();

    m_dirtyRectangles.Destroy();
}
