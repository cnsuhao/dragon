#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\renderlayer2.h"
#include "UISDK\Kernel\Src\RenderLayer2\layer\windowrender.h"


namespace UI
{
IRenderLayer2::IRenderLayer2(RenderLayer2* p)
{
	m_pRenderLayer2Impl = p;
}
IRenderLayer2::~IRenderLayer2()
{
	
}
RenderLayer2*  IRenderLayer2::GetImpl()
{
	return m_pRenderLayer2Impl;
}
void  IRenderLayer2::SetOpacity(byte b)
{
	m_pRenderLayer2Impl->SetOpacity(b);
}
byte  IRenderLayer2::GetOpacity()
{
	return m_pRenderLayer2Impl->GetOpacity();
}
HDC   IRenderLayer2::GetHDC()
{
    return m_pRenderLayer2Impl->GetHDC();
}
void  IRenderLayer2::Rotate3D(int x, int y, int z)
{
	return m_pRenderLayer2Impl->Rotate3D(x, y , z);
}
void  IRenderLayer2::Rotate2D(int degree)
{
    return m_pRenderLayer2Impl->Rotate2D(degree);
}
void  IRenderLayer2::SetRotateCenterPos(ROTATE_CENTER_TYPE eType, int xOffset, int yOffset)
{
    return m_pRenderLayer2Impl->SetRotateCenterPos(eType, xOffset, yOffset);
}
void  IRenderLayer2::DirectCommit(bool bUpdate)
{
    return m_pRenderLayer2Impl->DirectCommit(bUpdate);
}
IRenderTarget*  IRenderLayer2::GetRenderTarget()
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
GRAPHICS_RENDER_LIBRARY_TYPE  IWindowRender::GetGraphicsRenderType()
{
    return m_pWindowRenderImpl->GetGraphicsRenderType();
}
void  IWindowRender::SetGraphicsRenderType(GRAPHICS_RENDER_LIBRARY_TYPE  eTpye)
{
	return m_pWindowRenderImpl->SetGraphicsRenderType(eTpye);
}
}