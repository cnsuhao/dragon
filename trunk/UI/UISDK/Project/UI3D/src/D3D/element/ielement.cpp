#include "stdafx.h"
#include "UISDK\Project\UI3D\inc\ielement.h"
#include "UISDK\Project\UI3D\src\D3D\element\element.h"
#include "UISDK\Project\UI3D\src\D3D\element\rectangleelement.h"
#include "UISDK\Project\UI3D\src\D3D\element\imageelement.h"
#if 0
#include "UISDK\Project\UI3D\src\Element\mesh\meshelem.h"
#include "UISDK\Project\UI3D\src\Element\particle\particle.h"
#endif
namespace UI
{

IElement::IElement(Element* pImpl)
{
	m_pElementImpl = pImpl;
}

void  IElement::MoveWorld(float x, float y, float z)
{
	m_pElementImpl->GetWorldMatrix()->Move(x, y, z);
}
void  IElement::MoveWorldBy(float x, float y, float z)
{
    m_pElementImpl->GetWorldMatrix()->MoveBy(x, y, z);
}

void  IElement::RotateWorld(float x, float y, float z)
{
	m_pElementImpl->GetWorldMatrix()->Rotate(x, y, z);
}
void  IElement::RotateWorldBy(float x, float y, float z)
{
    m_pElementImpl->GetWorldMatrix()->RotateBy(x, y, z);
}

void  IElement::GetPos(float* px, float* py, float* pz)
{
    m_pElementImpl->GetWorldMatrix()->GetPos(px, py, pz);
}
void  IElement::GetRotate(float* px, float* py, float* pz)
{
    m_pElementImpl->GetWorldMatrix()->GetRotate(px, py, pz);
}
void  IElement::GetScale(float* px, float* py, float* pz)
{
    m_pElementImpl->GetWorldMatrix()->GetScale(px, py, pz);
}

IRectangleElement::IRectangleElement(RectangleElement* pImpl) : IElement(static_cast<Element*>(pImpl))
{
    m_pRectangleElementImpl = pImpl;
}
void  IRectangleElement::SetRect(RECTF* prc, float z)
{
    m_pRectangleElementImpl->CreateBuffer(prc, z);  // TODO: add update method
}


IImageElement::IImageElement(ImageElement* pImpl) : IElement(static_cast<Element*>(pImpl))
{
    m_pImageElementImpl = pImpl;
}
// bool  IImageElement::LoadFromFile(const TCHAR* szText)
// {
//     return m_pImageElementImpl->LoadFromFile(szText);
// }
// void  IImageElement::SetDrawRect(float fLeft, float fTop, float fRight, float fBottom, float fZ)
// {
//     m_pImageElementImpl->SetDrawRect(fLeft, fTop, fRight, fBottom, fZ);
// }
// void  IImageElement::SetDirectDrawRect(RECT* prc, float fz)
// {
//     m_pImageElementImpl->SetDirectDrawRect(prc, fz);
// }
// void  IImageElement::SetTransparency(float fAlpha)
// {
//     m_pImageElementImpl->SetTransparency(fAlpha);
// }


#if 0
IMeshElement::IMeshElement(MeshElement* pImpl) : IElement(static_cast<Element*>(pImpl))
{
	m_pMeshElementImpl = pImpl;
}
bool  IMeshElement::LoadFromFile(const TCHAR* szText)
{
	return m_pMeshElementImpl->GetMesh()->LoadFromXFile(szText);
}

IParticleElement::IParticleElement(ParticleElement* pImpl) : IElement(static_cast<Element*>(pImpl))
{
	m_pParticleElementImpl = pImpl;
}
bool  IParticleElement::SetTextureCount(unsigned int nTextureCount)
{
	return m_pParticleElementImpl->SetTextureCount(nTextureCount);
}
bool  IParticleElement::SetTexture(unsigned int nIndex, const TCHAR* szFile)
{
	return m_pParticleElementImpl->SetTexture(nIndex, szFile);
}
void  IParticleElement::InitParticles(unsigned int nCacheCount, unsigned int nAliveCount)
{
	m_pParticleElementImpl->InitParticles(nCacheCount, nAliveCount);
}
#endif
}

