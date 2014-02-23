#include "stdafx.h"
#include "meshelem.h"
#include "UISDK\Project\UI3D\inc\ielement.h"


namespace UI
{
MeshElement::MeshElement() : m_mesh(GetDevice())
{
	m_pIMeshElement = NULL;
}

MeshElement::~MeshElement()
{
	SAFE_DELETE(m_pIMeshElement);
}	

DxMesh*  MeshElement::GetMesh()
{
    return &m_mesh;
}
IMeshElement*  MeshElement::GetIMeshElement()
{
	if (m_pIMeshElement)
		return m_pIMeshElement;

	m_pIMeshElement = new IMeshElement(this);
	return m_pIMeshElement;
}

void MeshElement::OnPaint()
{
	m_matWorld.ApplyDevice(GetDevice());
    m_mesh.Render();
}

}