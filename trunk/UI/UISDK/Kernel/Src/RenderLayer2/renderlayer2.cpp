#include "stdafx.h"
#include "renderlayer2.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"

namespace UI
{
    RenderLayer2::RenderLayer2()
    {
        m_lFlags = RENDER_LAYER2_DIRTY;
        m_pObjCreate = NULL;

        m_pParentLayer = NULL;
        m_pPrevLayer = NULL;
        m_pNextLayer = NULL;
        m_pFirstChildLayer = NULL;
    }
    RenderLayer2::~RenderLayer2()
    {

    }

    bool  RenderLayer2::IsDirty()
    {
        if (m_lFlags & RENDER_LAYER2_DIRTY)
            return true;
        return false;
    }

    void  RenderLayer2::SetDirty(bool b)
    {
        if (b)
        {
            m_lFlags |= RENDER_LAYER2_DIRTY;
        }
        else
        {
            m_lFlags &= ~RENDER_LAYER2_DIRTY;
        }
    }
}
