#include "stdafx.h"
#include "UISDK\Project\UI3D\inc\istage3d.h"
#include "UISDK\Project\UI3D\Src\D3D\stage3d.h"
#include "UISDK\Project\UI3D\src\Element\mesh\meshelem.h"
#include "UISDK\Project\UI3D\src\Element\particle\particle.h"
#include "UISDK\Project\UI3D\src\Element\image\imageelement.h"

namespace UI
{
UI_IMPLEMENT_Ixxx_INTERFACE2(IStage3D, Stage3D, IPanel)

void  IStage3D::SetClearColor(DWORD dwColor)
{
    m_pStage3DImpl->SetBkgndColor(dwColor);
}
DWORD  IStage3D::GetClearColor()
{
    return m_pStage3DImpl->GetBkgndColor();
}

IMeshElement*  IStage3D::AddMeshElement()
{
	MeshElement* pElem = new MeshElement;
	if (!m_pStage3DImpl->AddElement(pElem))
	{
		delete pElem;
		return NULL;
	}

	return pElem->GetIMeshElement();
}

IParticleElement*  IStage3D::AddParticleElement()
{
	ParticleElement* pElem = new ParticleElement;
	if (!m_pStage3DImpl->AddElement(pElem))
	{
		delete pElem;
		return NULL;
	}

	return pElem->GetIParticleElement();
}

IImageElement*  IStage3D::AddImageElement()
{
    ImageElement* pElem = new ImageElement;
    if (!m_pStage3DImpl->AddElement(pElem))
    {
        delete pElem;
        return NULL;
    }

    return pElem->GetIImageElement();
}

void  IStage3D::GetFullWorldCoord(float fzWorld, float*  pfWidth, float* pfHeight)
{
	m_pStage3DImpl->GetFullWorldCoord(fzWorld, pfWidth, pfHeight);
}

void  IStage3D::EnableCamera(bool b)
{
	m_pStage3DImpl->EnableCamera(b);
}
bool  IStage3D::IsCameraEnable()
{
	return m_pStage3DImpl->IsCameraEnable();
}
}