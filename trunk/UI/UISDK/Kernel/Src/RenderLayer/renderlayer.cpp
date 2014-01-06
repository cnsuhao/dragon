#include "stdafx.h"
#include "renderlayer.h"

namespace UI
{

RenderLayer::RenderLayer()
{
    m_pIRenderLayer = NULL;
	m_pWindow = NULL;
	m_pNextLayer = NULL;
	m_pPrevLayer = NULL;
	m_nZorder = 0;
	m_pRenderChain = NULL;
	m_bLayerEnable = true;

    m_pRenderTarget = NULL;
    m_hMemDC = NULL;
}
RenderLayer::~RenderLayer()
{
	SAFE_DELETE(m_pRenderTarget);
}
RenderLayer*  RenderLayer::GetNextLayer()
{
	return m_pNextLayer;
}
RenderLayer*  RenderLayer::GetPrevLayer()
{
	return m_pPrevLayer;
}
void  RenderLayer::SetNextLayer(RenderLayer* p)
{
	m_pNextLayer = p;
}
void  RenderLayer::SetPrevLayer(RenderLayer* p)
{
	m_pPrevLayer = p;
}

void  RenderLayer::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
    __super::SetAttribute(pMapAttrib, bReload);
    pMapAttrib->GetAttr_int(XML_RENDERCHAIN_Z_ORDER, true, &m_nZorder);
}

}