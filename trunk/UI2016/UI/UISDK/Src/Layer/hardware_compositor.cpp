#include "stdafx.h"
#if 0
#include "hardware_compositor.h"
#include "hardware_layer.h"
#include "UISDK/Project/UICompositor/Inc/ihardwarecompositor.h"


HardwareCompositor::HardwareCompositor()
{
    m_pHardwareComposition = NULL;
}

HardwareCompositor::~HardwareCompositor()
{
    SAFE_RELEASE(m_pHardwareComposition);
}

void  HardwareCompositor::virtualBindHWND(HWND hWnd)
{
    assert (!m_pHardwareComposition);
    m_pHardwareComposition = UI::CreateHardwareComposition(m_hWnd);
}

void  HardwareCompositor::Resize(uint nWidth, uint nSize)
{
    assert (m_pHardwareComposition);
    if (m_pHardwareComposition)
        m_pHardwareComposition->Resize(nWidth, nSize);
}

Layer* HardwareCompositor::virtualCreateLayer()
{
    return new HardwareLayer();
}

// 硬件合成只能是每个层分别去调用updatedirty，而不是像软件渲染一样由parent object遍历child时去调用
// updatedirty。因为硬件下父layer可能没有dirty，而子layer有dirty.
void  HardwareCompositor::UpdateDirty(__out_opt RectArray& arrDirtyInWindow)
{
    if (!m_pRootLayer)
        return;

    Layer* p = m_pRootLayer;
    //p->UpdateDirty();

    while (p)
    {
        update_dirty_recursion(p);
        p = p->GetNext();
    }
}

void  HardwareCompositor::update_dirty_recursion(Layer* p)
{
    assert (p);

    static_cast<HardwareLayer*>(p)->UpdateDirty();

    p = p->GetFirstChild();
    while (p)
    {
        update_dirty_recursion(p);
        p = p->GetNext();
    }
}

void  HardwareCompositor::Commit(HDC hDC, const RectArray& arrDirtyInWindow) 
{
    // FillRect(hDC, prcInWindow, (HBRUSH)GetStockObject(GRAY_BRUSH));

    if (!m_pHardwareComposition)
        return;
    if (!m_pRootLayer)
        return;

    if (!m_pHardwareComposition->BeginCommit())
        return;

	GpuLayerCommitContext context;
    Layer* p = m_pRootLayer;
    while (p)
    {
        commit_recursion(p, &context);
        p = p->GetNext();
    }

    m_pHardwareComposition->EndCommit();
}

void  HardwareCompositor::commit_recursion(Layer* p, GpuLayerCommitContext* pContext)
{
    if (!p->GetContent()->IsSelfVisible())
        return;

	GpuLayerCommitContext context(*pContext);
    static_cast<HardwareLayer*>(p)->Commit(&context);

    p = p->GetFirstChild();
    while (p)
    {
        commit_recursion(p, &context);
        p = p->GetNext();
    }
}

IGpuRenderLayer*  HardwareCompositor::CreateGpuLayerTexture()
{
    assert(m_pHardwareComposition);
    if (!m_pHardwareComposition)
        return NULL;

    IGpuRenderLayer* pGpuTexture = NULL;
    if (m_pHardwareComposition)
    {
        pGpuTexture = m_pHardwareComposition->CreateLayerTexture();
    }

    return pGpuTexture;
}
#endif