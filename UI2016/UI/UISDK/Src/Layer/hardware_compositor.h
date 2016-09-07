#pragma once
#include "compositor.h"
#if 0
namespace UI
{
interface IHardwareComposition;
class GpuLayerCommitContext;

class HardwareCompositor : public Compositor
{
public:
    HardwareCompositor();
    ~HardwareCompositor();

    virtual void  virtualBindHWND(HWND) override;

    virtual void  UpdateDirty(__out_opt  RectArray& arrDirtyInWindow) override;
    virtual void  Commit(HDC hDC, const RectArray& arrDirtyInWindow) override;
    virtual void  Resize(uint nWidth, uint nSize) override;

    virtual Layer*  virtualCreateLayer() override;

    IGpuRenderLayer*  CreateGpuLayerTexture();

private:
    void  draw_full_recursion(Layer* p);
    void  update_dirty_recursion(Layer* p);
    void  commit_recursion(Layer* p, GpuLayerCommitContext* pContext);

private:
    // ´°¿Ú½»»»Á´
    UI::IHardwareComposition*  m_pHardwareComposition;
};

}
#endif