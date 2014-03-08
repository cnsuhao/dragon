#include "stdafx.h"
#include "renderchain2.h"
#include "renderlayer2.h"
// http://www.chromium.org/developers/design-documents/gpu-accelerated-compositing-in-chrome

namespace UI
{
    RenderChain2::RenderChain2()
    {
        m_pRootLayer = NULL;
    }
    RenderChain2::~RenderChain2()
    {

    }

    RenderLayer2*  RenderChain2::CreateRenderLayer(Object*  pObj, int zOrder)
    {
        RenderLayer2*  p = new RenderLayer2();

        return p;
    }
    void  RenderChain2::DestroyRenderLayer(RenderLayer2*  pLayer)
    {
        
    }
}