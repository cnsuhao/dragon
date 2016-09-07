#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\renderlayer.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\windowrender.h"


using namespace UI;

IRenderLayer::IRenderLayer(RenderLayer* p)
{
	m_pRenderLayer2Impl = p;
}
IRenderLayer::~IRenderLayer()
{
	
}
RenderLayer*  IRenderLayer::GetImpl()
{
	return m_pRenderLayer2Impl;
}
void  IRenderLayer::SetAlwaysUpdate(bool b)
{
    m_pRenderLayer2Impl->SetAlwaysUpdate(b);
}
void  IRenderLayer::SetOpacity(byte b)
{
	m_pRenderLayer2Impl->SetOpacity(b);
}
byte  IRenderLayer::GetOpacity()
{
	return m_pRenderLayer2Impl->GetOpacity();
}

void  IRenderLayer::DirectCommit(bool bUpdate)
{
    return m_pRenderLayer2Impl->DirectCommit(bUpdate);
}
IRenderTarget*  IRenderLayer::GetRenderTarget()
{
    return m_pRenderLayer2Impl->GetRenderTarget();
}

IWindowRender::IWindowRender(WindowRender* p)
{
    m_pWindowRenderImpl = p;
}
IWindowRender::~IWindowRender()
{

}

WindowRender*  IWindowRender::GetImpl()
{
    return m_pWindowRenderImpl;
}
void  IWindowRender::SetCanCommit(bool b)
{
    m_pWindowRenderImpl->SetCanCommit(b);
}
void  IWindowRender::Commit(HDC hDC, RECT* prc, int nCount)
{
    m_pWindowRenderImpl->Commit(hDC, prc, nCount);
}
GRAPHICS_RENDER_LIBRARY_TYPE  IWindowRender::GetGraphicsRenderType()
{
    return m_pWindowRenderImpl->GetGraphicsRenderType();
}
void  IWindowRender::SetGraphicsRenderType(GRAPHICS_RENDER_LIBRARY_TYPE  eTpye)
{
	return m_pWindowRenderImpl->SetGraphicsRenderType(eTpye);
}
bool  IWindowRender::GetRequireAlphaChannel()
{
    return m_pWindowRenderImpl->GetRequireAlphaChannel();
}

void  IRenderLayer::CreateTransform(TRANSFORM_TYPE eType)
{
	m_pRenderLayer2Impl->CreateTransform(eType);
}
IRenderLayerTransform2D*  IRenderLayer::GetTransform2D(bool bForceCreate)
{
	return m_pRenderLayer2Impl->GetTransform2D(bForceCreate);
}
IRenderLayerTransform3D*  IRenderLayer::GetTransform3D(bool bForceCreate)
{
	return m_pRenderLayer2Impl->GetTransform3D(bForceCreate);
}
